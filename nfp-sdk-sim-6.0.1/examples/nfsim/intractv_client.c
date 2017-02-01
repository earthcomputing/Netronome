/*
 * Simulator Client interface for interactive client
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <nfp-common/ns_log.h>
#include <nfp-common/nfp_resid.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>
#include <nfp_bulk.h>

#include "interactive.h"

#include "simevent_names.h"
#include "read_mecsr.h"

/*********************************************************************
 * Defines                                                           *
 *********************************************************************/

#define ME_USTORE_SIZE (8 * 1024)
#define DEFAULT_USTORE_HEX "/tmp/uc"
#define DEFAULT_USTORE_LIST "/tmp/list"

/*********************************************************************
 * Globals                                                           *
 *********************************************************************/

/* use one scratch for all calls, safe for single-threaded */
static uint64_t uint64args[128];

/*********************************************************************
 * Simulator events                                                  *
 *********************************************************************/

static char *lookup_ev_type(uint32_t ev_type)
{
    int i;
    const char *ev_type_name = NULL;

    i = 0;
    while (1) {
        ev_type_name = all_simevent_names[i].name;
        if (!ev_type_name)
            break;

        if (ev_type == all_simevent_names[i].type)
            return (char *)ev_type_name;

        i++;
    }
    return NULL;
}



/***** Simulator event handling *****/
void client_event_handler(struct nfp_device *dev,
                          uint32_t ev_type,
                          void *ev_data,
                          uint32_t ev_data_len)
{
    /* work around a glitch with time deltas */
    static int ignore_delta = 2;
    struct interactive_data *d = interactive_glob;
    char *ev_type_name = NULL;
    char *sub_ev_type_name = NULL;
    char *ev_run_stop_reason_name = NULL;
    uint64_t sps = -1;
    uint64_t me_clock_count;
    uint64_t watch_id = -1;
    uint64_t value = -1;
    uint8_t br_type = -1;
    long int tusec = 0;
    float sec = 0;

    //int retval;
    //uint32_t value2;

    gettimeofday(&d->evt_new_time, NULL);

    /* Get the ME clock count with certain events */
    switch (ev_type) {
    case SIMEVENT_WATCH:
        {
            struct simevent_watch *evt = ev_data;
            me_clock_count = evt->me_clock_count;
            watch_id = evt->watchid;
            value = evt->generic_arg0;
            br_type = evt->watchclass;
            if (watch_id == INST_STEP_BREAKID)
                d->bp_inst_step_busy = 0;
        }
        break;
    case SIMEVENT_SIM_SHUTDOWN:
        {
            struct simevent_sim_shutdown *evt = ev_data;
            me_clock_count = evt->me_clock_count;
            printf("Got sim shutdown event, closing application\n");
            interactive_shutdown();
            return;
        }
        break;
    case SIMEVENT_RUN_START:
        {
            struct simevent_run_start *evt = ev_data;
            me_clock_count = evt->me_clock_count;
        }
        break;
    case SIMEVENT_RUN_UPDATE:
        {
            struct simevent_run_update *evt = ev_data;
            tusec = (d->evt_new_time.tv_sec * 1000000 + d->evt_new_time.tv_usec) -
                    (d->evt_old_time.tv_sec * 1000000 + d->evt_old_time.tv_usec);
            sec = ((float)tusec) / (1000*1000);
            me_clock_count = evt->me_clock_count;
            memcpy(&d->evt_old_time, &d->evt_new_time, sizeof(struct timeval));
        }
        break;
    case SIMEVENT_RUN_STOP:
        {
            struct simevent_run_stop *evt = ev_data;
            me_clock_count = evt->me_clock_count;
            ev_run_stop_reason_name = lookup_ev_type(evt->stop_reason);
        }
        break;
    case SIMEVENT_RUN_RESET:
        {
            struct simevent_run_reset *evt = ev_data;
            me_clock_count = evt->me_clock_count;
        }
        break;
    case SIMEVENT_RUN_RESETDONE:
        {
            struct simevent_run_resetdone *evt = ev_data;
            me_clock_count = evt->me_clock_count;
        }
        break;
    default:
        me_clock_count = -1;
    }

    /* if we dont display anything finish now */
    if (!d->evt_display)
        return;

    /* Get the name of the event */
    ev_type_name = lookup_ev_type(ev_type);

    if (ev_type_name != NULL) {
        if (ev_run_stop_reason_name == NULL) {
            printf("got event[%d:%s] @%p [%u bytes]", ev_type, ev_type_name, ev_data, ev_data_len);
        } else {
            printf("got event[%d:%s:%s] @%p [%u bytes]", ev_type, ev_type_name, ev_run_stop_reason_name,  ev_data, ev_data_len);
        }
        /* See if we have a SIM break or change event */
        if (watch_id != -1) {
            /* Check if we name a name for the SIMEVENT br_type */
            sub_ev_type_name = lookup_ev_type(br_type);
            if (sub_ev_type_name == NULL) {
                printf(" Watch ID:%lu value:%"PRIx64" type:%d clock:%lu",
                       watch_id, value, (int) br_type, me_clock_count);
            } else {
                printf(" Watch ID:%lu value:%"PRIx64" type:[%d:%s] clock:%lu",
                       watch_id, value, (int) br_type, sub_ev_type_name, me_clock_count);
            }
            /* Force the me_clock_count to -1 to correct update for sps */
            me_clock_count = -1;
        }
        /* See if we have a SIM event that gives the me_clock_count and calculate the Sps */
        if (me_clock_count != -1) {
            sps = me_clock_count - d->evt_old_clk_space;
            d->evt_old_clk_space = me_clock_count;
            if (ignore_delta > 0) {
                sec = 0;
                ignore_delta--;
            }
            printf(" RUN clock %lu Sps:%lu (update interval %f sec)\n",
                   me_clock_count, sps, (float) sec);
        } else {
            printf("\n");
        }
    } else {
        printf("got event[%d] @%p [%u bytes]\n", ev_type, ev_data, ev_data_len);
    }
}

/*********************************************************************
 * Support functions                                                 *
 *********************************************************************/

static int ibuf2uc(uint64_t *uc, unsigned char *buf, int len)
{
    int i = 0;
    int windex =  0;
    uint64_t word;
    unsigned char *val_str = NULL;

    i = 0;
    val_str = buf;
    for (i = 0; i < len && windex < 1024*8; i++) {
        if (buf[i] == '\r' || buf[i] == '\n' ||
                buf[i] == ' ' || buf[i] == '\t') {
            buf[i] = '\0';
            if (strlen((char *)val_str) > 0) {
                word = strtol((char *)val_str, NULL, 16);
                printf("%3d: %012"PRIx64"\n", windex, word);
                uc[windex++] = word;
            }
            val_str = &buf[i + 1];
            continue;
        }
        if (buf[i] >= '0' && buf[i] <= '9')
            continue;
        if (buf[i] >= 'a' && buf[i] <= 'f')
            continue;
        if (buf[i] >= 'A' && buf[i] <= 'F')
            continue;

        fprintf(stderr, "invalid character in uc file (%c)\n", buf[i]);
    }
    return windex;
}


/* Read micro-code hex file */
static int read_uc_hex(const char *fname, uint64_t *uc)
{
    /* big enough to hold all the strings */
    unsigned char buf[ME_USTORE_SIZE * 128];
    int fd, len;

    if (!fname)
        fname = DEFAULT_USTORE_HEX;

    printf("Reading file %s\n", fname);

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open file %s\n", fname);
        return -1;
    }
    len = read(fd, buf, sizeof(buf));
    close(fd);

    if (len < 0) {
        fprintf(stderr, "read file failed\n");
        return -1;
    }
    printf("loading %d bytes\n", len);
    len = ibuf2uc(uc, buf, len);
    return len;
}

/* Read micro-code from list file */
static int read_uc_list(const char *fname, uint64_t *uc, int uclen)
{
    FILE *fd;
    int len=0;
    char buf[512];
    char *itok;
    char *itokn;

    if (!fname)
        fname = DEFAULT_USTORE_LIST;

    printf("Reading file \"%s\" \n",fname);
    fd = fopen(fname, "r");
    if (fd == NULL) {
        fprintf(stderr, "failed to open file %s\n", fname);
        perror("Error");
        return -1;
    }

    while (fgets(buf, sizeof(buf), fd) != NULL) {
        printf("%s", buf);
        if (buf[0] != '.')
            continue;
        printf("%s", buf);
        itok = strtok(buf, " ");
        if (!itok)
            continue;

        itokn = strtok(NULL, " ");
        if (!itokn)
            continue;

        itok = strtok(NULL, " ");
        if (!itok)
            continue;

        if (!strncmp(itok, "common_code", 11)) {
            uc[len] = strtoul(itokn, NULL, 16);
            printf("* Instruction %d: %"PRIx64"\t", len, uc[len]);
            /* print_instruction(uc[len]); */
            len++;
        }
    }

    fclose(fd);
    printf("Read %d instructions\n", len);
    return len;
}

/***** Helper functions *****/
int mecsr_read(struct interactive_data *d, int ctx, int csr, uint32_t *val)
{
    if (nfp_mecsr_read(d->h_nfp, d->cmd_meid, ctx, csr, val) < 0) {
        fprintf(stderr, "mecsr read to %x failed (%s)\n", csr, strerror(errno));
        return -1;
    }
    return 0;
}

int mecsr_write(struct interactive_data *d, int ctx, int csr, uint32_t val)
{
    if (nfp_mecsr_write(d->h_nfp, d->cmd_meid, ctx, csr, val) < 0) {
        fprintf(stderr, "mecsr write to %x failed (%s)\n", csr, strerror(errno));
        return -1;
    }
    return 0;
}

/***** Argument Handling *****/

/* Convert a string to an array of uin64ts */
static int get_args_uint64(const char *str, uint64_t *out, int max)
{
    const char *s;
    char val_str[64];
    int val_off = 0;
    int got_val = 0;
    int n = 0;
    int done = 0;

    s = str;
    while (!done) {
        switch (*s) {
        case '\0':
        case '\r':
        case '\n':
            done = 1;
        case '\t':
        case ' ':
            if (got_val) {
                val_str[val_off] = '\0';
                val_off = 0;
                got_val = 0;
                out[n++] = strtoull(val_str, NULL, 0);
            }

            if (max == n) {
                done = 1;
            }
            break;
        default:
            if (val_off < 63) {
                val_str[val_off++] = *s;
                got_val = 1;
            }
            break;
        }
        s++;
    }
    return n;
}

/*********************************************************************
 * Exported Command functions                                        *
 *********************************************************************/

static int set_meid(struct interactive_data *d, const char *argstr)
{
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;
    int meid = d->cmd_meid;
    int ret, cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt == 2) {
        meid = NFP6000_MEID(uint64args[0], uint64args[1]);
    } else if (cnt == 1) {
        ret = nfp6000_idstr2meid(argstr, NULL);
        if (ret < 0) {
            meid = uint64args[0];
        } else {
            meid = ret;
        }
    }

    if (!NFP6000_MEID_IS_VALID(meid)) {
        printf("Invalid MEID: %x\n", meid);
        return -1;
    }

    /* Get the alt name for the MEID */
    meid_str_ptr = nfp6000_meid2str(meid_str, meid);
    if (meid_str_ptr == NULL) {
        meid_str[0] = 0;
    }
    printf("Current MEID:%s ", meid_str_ptr);
    meid_str_ptr = nfp6000_meid2altstr(meid_str, meid);
    if (meid_str_ptr == NULL) {
        meid_str[0] = 0;
    }
    printf("(%s) (0x%x)\n", meid_str, meid);

    d->cmd_meid = meid;

    return 0;
}

/** ME CSR Ops **/

int write_mecsr(struct interactive_data *d, const char *argstr)
{
    int rc;

    uint64args[2] = -1;
    rc = get_args_uint64(argstr, uint64args, 128);
    if (rc < 2) {
        fprintf(stderr, "CSR address and data not supplied\n");
        return -1;
    }
    printf("\nME CSR write 0x%08lx: 0x%08lx\n", uint64args[0], uint64args[1]);
    if (mecsr_write(d, uint64args[2], uint64args[0], uint64args[1]))
        return -1;

    return 0;
}

int read_mecsr(struct interactive_data *d, const char *argstr)
{
    uint32_t val;
    int rc;

    /* Read CSR */
    uint64args[1] = -1;
    rc = get_args_uint64(argstr, uint64args, 128);
    if (rc < 1) {
        fprintf(stderr, "CSR address not supplied\n");
        return -1;
    }
    if (mecsr_read(d, uint64args[1], uint64args[0], &val))
        return -1;
    printf("\nME CSR read 0x%08lx: 0x%08x\n", uint64args[0], val);

    return 0;
}

int write_me_ctx_enable(struct interactive_data *d, const char *argstr)
{
    printf("\nWrite ME CTX enable\n");
    /* XXX Clear Breakpoint bit also?? */
    if (mecsr_write(d, -1, NFP_ME_CtxEnables, 0x0800ff00) < 0)
        return -1;
    printf("Enable Condition Codes\n");
    if (mecsr_write(d, -1, NFP_ME_CondCodeEn, (1 << 13)) < 0)
        return -1;
    printf("running all threads\n");
    return 0;
}

int read_me_ctx_enable(struct interactive_data *d, const char *argstr)
{
    uint32_t val;
    printf("\nRead ME CTX enable\n");
    if (mecsr_read(d, -1, NFP_ME_CtxEnables, &val) < 0)
        return -1;

    printf("CtxEnables = 0x%x\n", val);
    return 0;
}

int set_wakeup(struct interactive_data *d, const char *argstr)
{
    int i;

    printf("\nset voluntary bit for all me contexts\n");

    for (i = 0; i < 8; i++) {
        if (mecsr_write(d, i, NFP_ME_IndCtxWkpEvt, 1) < 0)
            return -1;
    }
    return 0;
}

int display_mailboxes(struct interactive_data *d, const char *argstr)
{
    int i, ret;
    uint32_t vals[4];
    uint32_t mecsrs[4];

    printf("\nRead mailboxes\n");
    /* use the bulk api to grab all at once */
    mecsrs[0] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox0);
    mecsrs[1] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox1);
    mecsrs[2] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox2);
    mecsrs[3] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox3);
    ret = nfp_bulk_mecsr_read(d->h_nfp, mecsrs, vals, 4);
    if (ret) {
        printf("error reading Mailboxs: %s\n", strerror(errno));
        return -1;
    }

    for (i = 0; i < 4; i++)
        printf("Mailbox%d = 0x%x\n", i, vals[i]);

    return 0;
}

int write_mailbox(struct interactive_data *d, const char *argstr)
{
    int cnt;
    int ret;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 2) {
        printf("\nNo mailbox number or value\n");
        return -1;
    }

    /* Write mailbox */
    ret = mecsr_write(d, -1, NFP_ME_Mailbox0 + uint64args[0] * 4, uint64args[1]);
    if (ret < 0) {
        printf("error writing Mailbox%d: %s\n",
               (int)uint64args[0], strerror(errno));
        return -1;
    }

    return 0;
}

/** ME Register Ops **/

int dump_mereg(struct interactive_data *d, const char *argstr, int type, int len)
{
    uint32_t val;
    int i;

    for (i = 0; i < len; i++) {
        if (nfp_me_register_read(d->h_nfp, d->cmd_meid,
                                 nfp_mereg(type, i), &val) < 0) {
            fprintf(stderr, "dump mereg failed (%s)\n", strerror(errno));
            return -1;
        }

        if (i % 8 == 0)
            printf("%08x: ", i * 4);
        printf("%08x ", val);
        if (i % 8 == 7)
            printf("\n");
    }

    return 0;
}

int dump_mereg_gpra(struct interactive_data *d, const char *argstr)
{
    printf("\nDumping GPRA:\n");
    nfp_emem_write(d->h_nfp, 24, NULL, 8, 8);
    return 0;
    return dump_mereg(d, argstr, NFP_MEREG_TYPE_GPRA, 128);
}
int dump_mereg_gprb(struct interactive_data *d, const char *argstr)
{
    printf("\nDumping GPRB:\n");
    return dump_mereg(d, argstr, NFP_MEREG_TYPE_GPRB, 128);
}
int dump_mereg_xferin(struct interactive_data *d, const char *argstr)
{
    printf("\nDumping XFER IN:\n");
    return dump_mereg(d, argstr, NFP_MEREG_TYPE_XFER_IN, 256);
}
int dump_mereg_xferout(struct interactive_data *d, const char *argstr)
{
    printf("\nDumping XFER OUT:\n");
    return dump_mereg(d, argstr, NFP_MEREG_TYPE_XFER_OUT, 256);
}
int dump_mereg_nn(struct interactive_data *d, const char *argstr)
{
    printf("\nDumping NN:\n");
    return dump_mereg(d, argstr, NFP_MEREG_TYPE_NN, 128);
}

int dump_lmem(struct interactive_data *d, const char *argstr)
{
    uint32_t val;
    int i;

    printf("\nDumping Local Memory:\n");
    for (i = 0; i < 128; i++) {
        if (nfp_lmem_read(d->h_nfp, d->cmd_meid, (void *) &val, 4, i * 4) < 0) {
            fprintf(stderr, "dump local memory failed (%s)\n", strerror(errno));
            return -1;
        }

        if (i % 8 == 0)
            printf("%08x: ", i * 4);
        printf("%08x ", val);
        if (i % 8 == 7)
            printf("\n");
    }

    return 0;
}

int write_mereg(struct interactive_data *d, const char *argstr,
                int type, int len)
{
    uint32_t val;
    int cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 2) {
        printf("Need to provide length offset and data\n");
        return -1;
    }

    printf("write 0x%08lx: 0x%08lx\n",
           uint64args[0], uint64args[1]);

    val = uint64args[1];

    if (nfp_me_register_write(d->h_nfp, d->cmd_meid,
                              nfp_mereg(type, uint64args[0]), val) < 0) {
        fprintf(stderr, "dump mereg failed (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int write_mereg_gpra(struct interactive_data *d, const char *argstr)
{
    printf("\nWriting to GPRA:\n");
    return write_mereg(d, argstr, NFP_MEREG_TYPE_GPRA, 128);
}
int write_mereg_gprb(struct interactive_data *d, const char *argstr)
{
    printf("\nWriting to GPRB:\n");
    return write_mereg(d, argstr, NFP_MEREG_TYPE_GPRB, 128);
}
int write_mereg_xferin(struct interactive_data *d, const char *argstr)
{
    printf("\nWriting to XFER IN:\n");
    return write_mereg(d, argstr, NFP_MEREG_TYPE_XFER_IN, 256);
}
int write_mereg_xferout(struct interactive_data *d, const char *argstr)
{
    printf("\nWriting to XFER OUT:\n");
    return write_mereg(d, argstr, NFP_MEREG_TYPE_XFER_OUT, 256);
}
int write_mereg_nn(struct interactive_data *d, const char *argstr)
{
    printf("\nWriting to NN:\n");
    return write_mereg(d, argstr, NFP_MEREG_TYPE_NN, 128);
}

int write_lmem(struct interactive_data *d, const char *argstr)
{
    uint32_t val;
    int cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 2) {
        printf("\nNeed to provide length offset and data\n");
        return -1;
    }

    printf("\nME Local Memory write 0x%08lx: 0x%08lx\n",
           uint64args[0], uint64args[1]);

    val = uint64args[1];
    if (nfp_lmem_write(d->h_nfp, d->cmd_meid,
                       (void *) &val, sizeof(val) , uint64args[0]) < 0) {
        fprintf(stderr, "error with lmem write to address %lx : %s\n",
                uint64args[0], strerror(errno));
        return -1;
    }

    return 0;
}

/** Ustore Ops **/

int dump_ustore(struct interactive_data *d, const char *argstr)
{
    uint64_t rmem[ME_USTORE_SIZE];
    int off = 0;
    int cnt;
    int i;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt == 1) /* optionally specify a word offset */
        off = uint64args[0];

    printf("\nRead micro store\n");
    if (nfp_ustore_read(d->h_nfp, d->cmd_meid, rmem, 1024*8*8, off * 8) < 0) {
        fprintf(stderr, "nfp_ustore_write() failed : %s\n", strerror(errno));
        return -1;
    }
    for (i = 0; i < 16; i+=4) {
        printf("%2d: ", i);
        printf("%016"PRIx64" ", *((uint64_t *)(rmem + (i + 0)*8)));
        printf("%016"PRIx64" ", *((uint64_t *)(rmem + (i + 1)*8)));
        printf("%016"PRIx64" ", *((uint64_t *)(rmem + (i + 2)*8)));
        printf("%016"PRIx64" ", *((uint64_t *)(rmem + (i + 3)*8)));
        printf("\n");
    }
    return 0;
}

int write_ustore_list(struct interactive_data *d, const char *argstr)
{
    uint64_t buf[ME_USTORE_SIZE];
    const char *filename;

    if (strlen(argstr) == 0) {
        fprintf(stderr, "no filename provided\n");
        return -1;
    }

    filename = argstr;

    printf("\nWrite micro store from list %s\n", filename);
    if (read_uc_list(filename, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "failed to load list file\n");
        return -1;
    }

    if (nfp_ustore_write(d->h_nfp, d->cmd_meid, buf, sizeof(buf), 0) < 0) {
        fprintf(stderr, "nfp_ustore_write() failed : %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int write_ustore_hex(struct interactive_data *d, const char *argstr)
{
    uint64_t buf[ME_USTORE_SIZE];
    const char *filename;

    if (strlen(argstr) == 0)
        filename = DEFAULT_USTORE_HEX;
    else
        filename = argstr;

    printf("\nWrite micro store from hex file %s\n", filename);

    /* Read micro store file name */
    if (read_uc_hex(filename, buf) < 0)
        return -1;

    if (nfp_ustore_write(d->h_nfp, d->cmd_meid, buf, sizeof(buf), 0) < 0) {
        fprintf(stderr, "nfp_ustore_write() failed : %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

/** XPP OPs **/

int read_xpb(struct interactive_data *d, const char *argstr)
{
    uint32_t val;
    int cnt;
    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 1) {
        fprintf(stderr, "\nmust include an address\n");
        return -1;
    }
    if (nfp_xpb_readl(d->h_nfp_cpp, uint64args[0], &val)) {
        fprintf(stderr, "error with xpb read to address %lx : %s\n",
                uint64args[0], strerror(errno));
        return -1;
    }
    printf("\nXPB read 0x%08lx: 0x%08x\n", uint64args[0], val);
    return 0;
}

int write_xpb(struct interactive_data *d, const char *argstr)
{
    int cnt;
    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 2) {
        fprintf(stderr, "\nmust include an address and data\n");
        return -1;
    }
    printf("\nXPB write 0x%08lx: 0x%08lx\n", uint64args[0], uint64args[1]);
    if (nfp_xpb_writel(d->h_nfp_cpp, uint64args[0], uint64args[1])) {
        fprintf(stderr, "error with xpb write to address %lx : %s\n",
                uint64args[0], strerror(errno));
        return -1;
    }

    return 0;
}

/** Run Control **/
int run_sim(struct interactive_data *d, const char *argstr)
{
    return nfp_sal_run(d->h_nfp);
}

int stop_sim(struct interactive_data *d, const char *argstr)
{
    return nfp_sal_stop(d->h_nfp, 0);
}

int step_sim(struct interactive_data *d, const char *argstr)
{
    int s;
    int cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt == 0)
        s = 1;
    else
        s = uint64args[0];

    return nfp_sal_clock_step(d->h_nfp, s, 0);
}

int step_sim_dump_mailbox(struct interactive_data *d, const char *argstr)
{
    int s;
    int cnt;
    int ret;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt == 0)
        s = 1;
    else
        s = uint64args[0];

    ret = nfp_sal_clock_step(d->h_nfp, s, 0);
    if (ret < 0) {
        fprintf(stderr, "step failed\n");
        return -1;
    }

    return display_mailboxes(d, "");
}

int step_instruction(struct interactive_data *d, const char *argstr)
{
    int watchtype;
    int ctx;
    int ret;

    uint64args[0] = 0xff; /* default context is any */
    get_args_uint64(argstr, uint64args, 128);

    watchtype = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_BREAK,
                                    NFP_SAL_WATCH_TYPE_CHANGE);

    if (uint64args[0] >= 0 && uint64args[0] < 8)
        ctx = 1 << uint64args[0];
    else
        ctx = -1;
    ret = nfp_sal_watch_add_pc(d->h_nfp,
                               watchtype,
                               INST_STEP_BREAKID,
                               d->cmd_meid,
                               ctx,
                               -1);
    if (ret < 0) {
        fprintf(stderr, "failed to add watch for step\n");
        return -1;
    }
    d->bp_inst_step_loaded = 1;
    d->bp_inst_step_busy = 1;
    nfp_sal_run(d->h_nfp); /* run until break reached */

    return 0;
}

int reset_sim_bsp(struct interactive_data *d, const char *argstr)
{
    return nfp_sal_reset(d->h_nfp, NFP_SAL_RESET_LEVEL_BSP, 0);
}

int reset_sim_chip(struct interactive_data *d, const char *argstr)
{
    return nfp_sal_reset(d->h_nfp, NFP_SAL_RESET_LEVEL_CHIP, 0);
}

/** Watch ops **/

int add_watch(struct interactive_data *d, const char *argstr,
              int value, int notify)
{
    uint32_t wtype;
    uint64_t wid, wval = 0, wmask = UINT64_MAX, woff, wlen = 8;
    int ret = -1;
    int wop;
    int cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (value) {
        if (cnt < 4) {
            fprintf(stderr, "missing arguments\n");
            return -1;
        }
        wval = uint64args[2];
        wmask = uint64args[3];
    } else {
        if (cnt < 2) {
            fprintf(stderr, "missing arguments\n");
            return -1;
        }
        if (cnt > 2)
            wlen = uint64args[2];
    }
    wop = uint64args[0];
    woff = uint64args[1];

    wid = d->cmd_watchid;

    printf("\nAdding %s watch - watch_id:%lu",
           value ? "VALUE" : "CHANGE", wid);
    if (notify) {
        wtype = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_NOTIFY,
                                        NFP_SAL_WATCH_TYPE_CHANGE);
        printf(" (NOTIFY)\n");
    } else {
        wtype = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_BREAK,
                                        NFP_SAL_WATCH_TYPE_CHANGE);
        printf(" (STOP)\n");
    }

    /* arg0: 0 mecsr, 2 gprA, 3 gprb, 4 xfer in, 5 xfer out, 6 NN, 7 cls */
    /* arg1: index */
    switch (wop) {
    case 0:
        ret = nfp_sal_watch_add_mecsr(d->h_nfp,
                                      wtype,
                                      wid,
                                      wmask,
                                      wval,
                                      d->cmd_meid,
                                      -1, /* ctx */
                                      woff);
        break;
    case 2:
        ret = nfp_sal_watch_add_me_register(d->h_nfp,
                                            wtype,
                                            wid,
                                            wmask,
                                            wval,
                                            d->cmd_meid,
                                            nfp_mereg(NFP_MEREG_TYPE_GPRA, woff));
        break;
    case 3:
        ret = nfp_sal_watch_add_me_register(d->h_nfp,
                                            wtype,
                                            wid,
                                            wmask,
                                            wval,
                                            d->cmd_meid,
                                            nfp_mereg(NFP_MEREG_TYPE_GPRB, woff));

        break;
    case 4:
        ret = nfp_sal_watch_add_me_register(d->h_nfp,
                                            wtype,
                                            wid,
                                            wmask,
                                            wval,
                                            d->cmd_meid,
                                            nfp_mereg(NFP_MEREG_TYPE_XFER_IN, woff));
        break;
    case 5:
        ret = nfp_sal_watch_add_me_register(d->h_nfp,
                                            wtype,
                                            wid,
                                            wmask,
                                            wval,
                                            d->cmd_meid,
                                            nfp_mereg(NFP_MEREG_TYPE_XFER_OUT, woff));
        break;
    case 6:
        ret = nfp_sal_watch_add_me_register(d->h_nfp,
                                            wtype,
                                            wid,
                                            wmask,
                                            wval,
                                            d->cmd_meid,
                                            nfp_mereg(NFP_MEREG_TYPE_NN, woff));

        break;
    case 7:
        if (value) {
            fprintf(stderr, "cls area watch not supported for value watch\n");
            return -1;
        }
        ret = nfp_sal_watch_add_cls_area(d->h_nfp,
                                         wtype,
                                         wid,
                                         NFP6000_MEID_ISLAND_of(d->cmd_meid),
                                         wlen,
                                         woff);
        break;
    case 8:
        ret = nfp_sal_watch_add_xpb(d->h_nfp,
                                    wtype,
                                    wid,
                                    wmask,
                                    wval,
                                    woff);
        break;
    default:
        fprintf(stderr, "invalid watch arg/operation\n");
        return -1;
    }

    if (ret < 0) {
        printf("failed to add watch (%s)\n", strerror(errno));
        return -1;
    }
    printf("Watch %ld added\n", wid);
    d->cmd_watchid++;

    return 0;
}

int add_watch_change(struct interactive_data *d, const char *argstr)
{
    return add_watch(d, argstr, 0, 0);
}

int add_watch_value(struct interactive_data *d, const char *argstr)
{
    return add_watch(d, argstr, 1, 0);
}

int add_watch_change_notify(struct interactive_data *d, const char *argstr)
{
    return add_watch(d, argstr, 0, 1);
}

int add_watch_value_notify(struct interactive_data *d, const char *argstr)
{
    return add_watch(d, argstr, 1, 1);
}

int del_watch(struct interactive_data *d, const char *argstr)
{
    int cnt;
    int ret;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt == 0) {
        fprintf(stderr, "need to provide watchid");
        return -1;
    }

    ret = nfp_sal_watch_del(d->h_nfp, uint64args[0]);
    if (ret < 0) {
        fprintf(stderr, "del watch failed (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int flush_watch(struct interactive_data *d, const char *argstr)
{
    /* flush all watches belonging to this client */
    return nfp_sal_watch_flush(d->h_nfp, 0);
}

/** Packet ops */

int packet_egress(struct interactive_data *d, const char *argstr)
{

    uint8_t mem[1024*8];
    uint64_t time;
    int i, n;
    int cnt;
    int nbi = 0, port = 0;
    int ret;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt > 0)
        nbi = uint64args[0];
    if (cnt > 1)
        port = uint64args[1];

    cnt = nfp_sal_packet_egress_status(d->h_nfp, nbi, port);
    if (cnt < 0) {
        fprintf(stderr, "failed to get egress status\n");
        return -1;
    }

    printf("\n%lu: egress q = %d\n",
            nfp_sal_packet_get_time(d->h_nfp),
            cnt);
    ret = nfp_sal_packet_egress_status(d->h_nfp, nbi, port);
    if (ret > 0) {
        n = nfp_sal_packet_egress(d->h_nfp, nbi, port,
                                  (void *) mem, sizeof(mem),
                                  &time);
        printf("new egress packet @ time %lu(size = %d bytes):\n", time, n);
        for (i = 0; i < n; i++) {
            printf("%02x ", (unsigned char) mem[i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");
    }

    if (ret < 0) {
        fprintf(stderr, "failed to get egress packet (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int packet_ingress(struct interactive_data *d, const char *argstr)
{
    static uint16_t val = 0; /* static counter */
    int cnt;
    int nbi = 0, port = 0, size = 200, numpkts = 1;
    uint16_t mem[1024*8];
    int i, p;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt > 0)
        nbi = uint64args[0];
    if (cnt > 1)
        port = uint64args[1];
    if (cnt > 2)
        numpkts = uint64args[2];
    if (cnt > 3)
        size = uint64args[3];

    if (size > sizeof(mem)) {
        fprintf(stderr, "invalid packet size\n");
        return -1;
    }

    for (p = 0; p < numpkts; p++) {
        for (i = 0; i < (size + 2) / 2; i++)
            mem[i] = val++;

        cnt = nfp_sal_packet_ingress_status(d->h_nfp, nbi, port);
        if (cnt < 0) {
            fprintf(stderr, "failed to get ingress status\n");
            return -1;
        }
    }

    /* Packet ingress */
    printf("\n%lu: ingress q = %d\n",
            nfp_sal_packet_get_time(d->h_nfp), cnt);
    if (nfp_sal_packet_ingress(d->h_nfp, nbi, port, (void *) mem, size, 0) < 0) {
        fprintf(stderr, "failed to get ingress failed (%s)\n", strerror(errno));
        return -1;
    }

    cnt = nfp_sal_packet_ingress_status(d->h_nfp, nbi, port);
    if (cnt < 0) {
        fprintf(stderr, "failed to get ingress status\n");
        return -1;
    }

    printf("%lu: ingress q = %d\n",
            nfp_sal_packet_get_time(d->h_nfp), cnt);

    return 0;
}

int toggle_event_display(struct interactive_data *d, const char *argstr)
{
    if (d->evt_display)
        d->evt_display = 0;
    else
        d->evt_display = 1;
    printf("event display is %s\n", d->evt_display ? "ON" : "OFF");
    return 0;
}

int connection_latency(struct interactive_data *d, const char *argstr)
{
    int ret;
    uint32_t vals[4];
    uint32_t mecsrs[4];
    struct timeval before, after;

    gettimeofday(&before, NULL);

    /* use the bulk api to grab all at once */
    mecsrs[0] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox0);
#if 0                
    mecsrs[1] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox1);
    mecsrs[2] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox2);
    mecsrs[3] = NFP_BULK_MECSR_MEID(d->cmd_meid) | NFP_BULK_MECSR_CTX(-1) |
                NFP_BULK_MECSR_CSR(NFP_ME_Mailbox3);
#endif               
    ret = nfp_bulk_mecsr_read(d->h_nfp, mecsrs, vals, 1);
    if (ret) {
        printf("error reading Mailboxs: %s\n", strerror(errno));
        return -1;
    }
    gettimeofday(&after, NULL);

    d->latency_usec =  (after.tv_sec * 1000000 + after.tv_usec) - (before.tv_sec * 1000000 + before.tv_usec);

    printf("Connection latency:~%.3fms\n", (float) (d->latency_usec/1000.0));

    return 0;
}

/*********************************************************************
 * Exported Command list                                             *
 *********************************************************************/

struct command_data client_cmds[] = {
    { "#", set_meid, MODE_ANY, "[<MEID> | <islnum> <d->cmd_meid> | <islname/num>.<d->cmd_meid>]\n"
                               "\t\tSet the selected ME or print current MEID if no args.\n"
                               "\t\t(i.e. # 32 0 or # 0x204 or # i32.me0 or # mei0.me0)"},
    { "c", write_mecsr,       MODE_ANY, "Write to an MECSR (args address, data)"},
    { "C", read_mecsr,        MODE_ANY, "Read from an MECSR (args address)"},
    { "1", set_wakeup,        MODE_ANY, "Set Voluntary Wakeup on all ME Contexts)"},
    { "M", display_mailboxes, MODE_ANY, "Read from an MECSR (args address)"},
    { "m", write_mailbox,     MODE_ANY, "Write to an MECSR (args address, data)"},

    { "Pa", dump_mereg_gpra,    MODE_ANY, "Print GPRA"},
    { "Pb", dump_mereg_gprb,    MODE_ANY, "Print GPRB"},
    { "Pi", dump_mereg_xferin,  MODE_ANY, "Print XFERIN"},
    { "Po", dump_mereg_xferout, MODE_ANY, "Print XFEROUT"},
    { "Pn", dump_mereg_nn,      MODE_ANY, "Print NN"},
    { "Pl", dump_lmem,          MODE_ANY, "Print Local Memory"},

    { "pa", write_mereg_gpra,    MODE_ANY, "Write GPRA (args address, data)"},
    { "pb", write_mereg_gprb,    MODE_ANY, "Write GPRB (args address, data)"},
    { "pi", write_mereg_xferin,  MODE_ANY, "Write XFERIN (args address, data)"},
    { "po", write_mereg_xferout, MODE_ANY, "Write XFEROUT (args address, data)"},
    { "pn", write_mereg_nn,      MODE_ANY, "Write NN (args address, data)"},
    { "pl", write_lmem,          MODE_ANY, "Write Local Memory (args address, data)"},

    { "U",  dump_ustore,         MODE_ANY, "Read micro store"},
    { "u",  write_ustore_hex,    MODE_ANY, "Write micro store from hex file (/tmp/uc if not specified)"},
    { "l",  write_ustore_list,   MODE_ANY, "Write micro store from list file"},

    { "x",  write_xpb, MODE_ANY, "Write to an xpb register (args address, data)"},
    { "X",  read_xpb,  MODE_ANY, "Read from an xpb register (arg address)"},

    { "R",  run_sim,                MODE_SIM, "Run simulator"},
    { "S",  stop_sim,               MODE_SIM, "Stop simulator"},
    { "s",  step_sim,               MODE_SIM, "Step simulator <arg steps (default 1)>"},
    { "t",  step_sim_dump_mailbox,  MODE_SIM, "Step cycles and read mailbox"},
    { "I",  step_instruction,       MODE_SIM, "Step a single ME instruction"},
    { "rb", reset_sim_bsp,          MODE_SIM, "Do a BSP level reset"},
    { "re", reset_sim_bsp,          MODE_SIM, "Do a BSP level reset"},
    { "rc", reset_sim_chip,         MODE_SIM, "Do a chip level reset"},

    { "b",  add_watch_change,        MODE_SIM, "add a change watch (stop) (arg offset [length] \n"
                                               "\t\twhere arg is 0 mecsr, 2 gpra, 3 gprb, 4 xfer in, 5 xfer out, 6 nn, 7 cls, 8 xpb)"},
    { "bn", add_watch_change_notify, MODE_SIM, "add a change watch (notify) (arg offset [length])"},
    { "v",  add_watch_value,         MODE_SIM, "add a value watch (stop) (arg offset value mask [length])"},
    { "vn", add_watch_value_notify,  MODE_SIM, "add a value watch (notify) (arg offset value mask [length])"},
    { "BN", del_watch,               MODE_SIM, "delete a watch (watchid)"},
    { "B",  flush_watch,             MODE_SIM, "flush all watches"},

    { "i", packet_ingress, MODE_SIM, "inject a packet <nbi, port, cnt, size>"},
    { "e", packet_egress,  MODE_SIM, "inject a packet <nbi, port>"},

    { "z", toggle_event_display, MODE_SIM, "Toggle event display"},

    { "lc", connection_latency, MODE_ANY, "Command latency\n"},

    {NULL}
};
