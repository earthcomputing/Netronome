#!/usr/bin/python
# Copyright (C) 2016 Netronome Systems, Inc.  All rights reserved.
import sys
import string

in_buckets = False
usage_error = True
if len(sys.argv) == 2:
    file = open(sys.argv[1])
    usage_error = False
elif len(sys.argv) == 3:
    file = open(sys.argv[2])
    if sys.argv[1] == '-s':
        in_buckets = True
        usage_error = False

if usage_error:
    print "usage: %s [options] filename\noptions:\n\t-s = skip searching for tags" % sys.argv[0]
    sys.exit(1)

bucket_count = 0
shared_lock_count = 0
exclusive_lock_count = 0
bad_head_count = 0
line_read = ''
line = ['','','','']
entry_count = [0, 0, 0, 0, 0, 0]

FC_LB_EMU = 2
FC_BUCKETS = (4 * 1024 * 1024 * FC_LB_EMU)
FC_OVERFLOW_ENTRIES = (3 * 1024 * 1024 * FC_LB_EMU)
FC_ENTRIES = (FC_BUCKETS + FC_OVERFLOW_ENTRIES)
MAX_ENTRY_INDEX =  FC_ENTRIES / FC_LB_EMU
STATS_MASK = 0xfffffbfe
ENTRY_MASK = 0x00ffffff

print "Checking with following settings:"
print "FC_LB_EMU: %d" % FC_LB_EMU
print "FC_BUCKETS: %d" %  FC_BUCKETS
print "FC_OVERFLOW_ENTRIES: %d" % FC_OVERFLOW_ENTRIES
print "FC_ENTRIES: %d" % FC_ENTRIES
print "MAX_ENTRY_INDEX: %d\n" % MAX_ENTRY_INDEX

def check_head(v):
    global bad_head_count
    if (v < 0) or (v >= MAX_ENTRY_INDEX):
        bad_head_count+=1
        return 1
    else:
        return 0

def dump_bucket(lines):
   for l in lines:
        print l

def print_report():
        print "\nTotals\n------\nBuckets checked: %d\nShared locks: %d\nExclusive Locks: %d\nBad Heads: %d\n" % (bucket_count,shared_lock_count, exclusive_lock_count, bad_head_count)

def print_histogram():
    extra = ""
    print "\nEntries\tDistribution\n----------------------------"
    if bucket_count > 0:
        for i in range(0, len(entry_count)):
            print "{}\t{:6.2f}% ({:10d})".format(i, (100 * entry_count[i]/float(bucket_count)), entry_count[i])
    else:
        print "Zero buckets"

for line_read in file:
    if 'BUCKETS_BASE' in line_read:
        print line_read.replace('\n','')
        in_buckets = True
    if 'ENTRIES_BASE' in line_read:
        in_buckets = False
        print_report()
    if in_buckets:
        vals_0 = line[0].split()
        if len(vals_0) == 5:
            addr = int(vals_0[0][:-1],16)
            if (addr % 64 == 0) and '*' not in line:
                bucket_count += 1
                display_bucket = False
                entries_per_bucket = 0
                for i in xrange(1,5):
                    h = vals_0[i]
                    v = int(h,16) >> 24
                    e = int(h,16) & ENTRY_MASK
                    if e != 0:
                        entries_per_bucket += 1
                    if v != 0:
                        timeout = (string.join(line[1].split()[1:3],'').replace('0x',''))[2*i:2*(i+1)]
                        if v & 0x80:
                            exclusive_lock_count += 1
                            shared_lock_count += v & 0x7f
                            print "%s: %s %s <-Exclusive" % (hex(addr),h,timeout)
                            display_bucket = True
                        else:
                            shared_lock_count  += v
                            print "%s: %s %s" % (hex(addr),h,timeout)
                            display_bucket = True
                vals_2 = line[2].split()
                vals_3 = line[3].split()

                # Test for overflow hash
                try:
                    h = vals_3[4]
                    e = int(h,16) & ENTRY_MASK
                    if e != 0:
                        entries_per_bucket += 1
                except IndexError:
                    pass

                # Histogram
                try:
                    entry_count[entries_per_bucket] += 1
                except IndexError:
                    raise

                # Head 1
                if int(vals_0[1],16) and (int(vals_2[1],16) & STATS_MASK):
                    print "%s: Stats not zero for Hash 1: %s %s" % (hex(addr),vals_0[1],vals_2[1])
                    display_bucket = True

                # Head 2
                idx = int(vals_3[2],16) >> 8
                if check_head(idx):
                    print "%s: Invalid Head 2: %d" % (hex(addr), idx)
                    display_bucket = True
                if (int(vals_0[2],16) == 0) != (idx == 0):
                    print "%s: Invalid Hash 2: %s vs Head 2: %x" % (hex(addr),vals_0[2], idx)
                    display_bucket = True
                if int(vals_0[2],16) and (int(vals_2[2],16) & STATS_MASK):
                    print "%s: Stats not zero for Hash 2: %s %s" % (hex(addr),vals_0[2],vals_2[2])
                    display_bucket = True


                # Head 3
                idx = ((int(vals_3[2],16) & 0xff) << 16) + (int(vals_3[3],16) >> 16)
                if check_head(idx):
                    print "%s: Invalid Head 3: %d" % (hex(addr), idx)
                    display_bucket = True
                if (int(vals_0[3],16) == 0) != (idx == 0):
                    print "%s: Invalid Hash 3: %s vs Head 3: %x" % (hex(addr),vals_0[3], idx)
                    display_bucket = True
                if int(vals_0[3],16) and (int(vals_2[3],16) & STATS_MASK):
                    print "%s: Stats not zero for Hash 3: %s %s" % (hex(addr),vals_0[3],vals_2[3])
                    display_bucket = True

                # Head 4
                idx = ((int(vals_3[3],16) & 0xffff) << 8) + (int(vals_3[4],16) >> 24)
                if check_head(idx):
                    print "%s: Invalid Head 4: %d" % (hex(addr), idx)
                    display_bucket = True
                if (int(vals_0[4],16) == 0) != (idx == 0):
                    print "%s: Invalid Hash 4: %s vs Head 4: %x" % (hex(addr),vals_0[4], idx)
                    display_bucket = True
                if int(vals_0[4],16) and (int(vals_2[4],16) & STATS_MASK):
                    print "%s: Stats not zero for Hash 4: %s %s" % (hex(addr),vals_0[4],vals_2[4])
                    display_bucket = True

                # Head OVF
                idx = int(vals_3[4],16) & 0xffffff
                if check_head(idx):
                    print "%s: Invalid Head ovf: %d" % (hex(addr), idx)
                if int(vals_3[1],16) & STATS_MASK:
                    print "%s: Stats not zero for OVF: %s %s" % (hex(addr),vals_3[1])
                    display_bucket = True

                if display_bucket:
                    dump_bucket(line)

    line = line[1:4]
    line.append(line_read.strip())

if in_buckets:
    print_report()
    print_histogram()

if not bucket_count:
    print "No buckets found. Does the file contain the BUCKETS_BASE marker? If not please use the -s option"

file.close()
