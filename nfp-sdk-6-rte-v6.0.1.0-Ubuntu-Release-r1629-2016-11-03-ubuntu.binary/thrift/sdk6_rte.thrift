/*
 * Copyright (C) 2015-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * This is the service for the SDK 6 Run Time Environment which 
 * allows interaction with a P4 platform 
 *
 * Version 6.0.0
 */

/**************************************************************
 * Global structures/defines                                  *
 **************************************************************/

// Generic return type
enum RteReturnValue {
    SUCCESS = 0,
    QUALIFIED,      // error reason string provided
    ARGINVALID,     // invalid argument
    IOERROR,        // platform IO error
    MEMORYERROR,    // memory allocation error
    FILEIOERROR,    // file IO error
    NOTLOADED,      // firmware not loaded
    HWINCOMPATIBLE, // platform doesn't support operation
}

struct RteReturn {
    1: required RteReturnValue value,
    2: optional string reason,
}

enum RteValueType {
    Invalid = 0,
    Int64 = 1,
    String = 2,
}

/* Structure for P4 Values
 * sometimes it will be more natural to store as i64 sometimes
 * as string
 */
struct RteValue {
    1:required RteValueType type, 
    2:optional string stringval, 
    3:optional i64 intval, 
}

/**************************************************************
 * Defines for general functions                              *
 **************************************************************/
enum LogLevel {
    Unknown = 0,
    Info = 1,
    Debug = 2,
    Extra = 3,
    Heavy = 4,
}
/**************************************************************
 * Structure for design load/unload/reload                    *
 **************************************************************/

struct DesignLoadArgs {
    1:required binary nfpfw, 
    2:required binary pif_design_json, 
    3:optional binary pif_config_json
}

/**************************************************************
 * Structure for Register                                     *
 **************************************************************/

struct RegisterArrayArg {
    1:optional i32 reg_id,
    2:optional i32 index,
    3:optional i32 count,
}

struct RegisterFieldDesc {
    1:optional string name,
    2:optional i32 width,
}

enum RegisterType {
    Invalid = 0,
    Global = 1,
    Direct = 2,
    Static = 3
}

struct RegisterDesc {
    /* The register name as defined in the P4 design */
    1:optional string name,
    /* A unique ID which is used to reference this register */
    2:optional i32 id,
    /* The type of the register */
    3:optional RegisterType type,
    /* The size of the register array:
     * In the case of RegisterDirect this is the size of the table
     * otherwise this is the the P4 instance count
     */
    4:optional i32 count,
    /* for RegisterDirect and RegisterStatic, the table name
     * associated with the register
     */
    5:optional string table,
    /* for RegisterDirect and RegisterStatic, the table identifier
     * associated with the register
     */
    6:optional i32 tableid,
    /* Fields associated with the register */
    7:optional list <RegisterFieldDesc> fields,

}

/**************************************************************
 * Structure for Counters                                     *
 **************************************************************/

enum P4CounterType {
    Invalid = 0,
    Global = 1,
    Direct = 2,
    Static = 3
}

struct P4CounterDesc {
    /* The counter name as defined in the P4 design */
    1:optional string name,
    /* A unique ID which is used to reference this counter */
    2:optional i32 id,
    /* The type of the counter */
    3:optional P4CounterType type,
    /* The size of the counter array:
     * In the case of P4CounterDirect this is the size of the table
     * otherwise this is the the P4 instance count
     */
    4:optional i32 count,
    /* for P4CounterDirect and P4CounterStatic, the table name
     * associated with the counter
     */
    5:optional string table,
    /* for P4CounterDirect and P4CounterStatic, the table identifier
     * associated with the counter
     */
    6:optional i32 tableid,
    /* the implemented width of the counter */
    7:optional i32 width,
}

struct SysCounterValue {
    1:optional string name,
    2:optional i32 id,
    3:optional RteValue value
}

struct P4CounterReturn {
    /* a buffer containing *count* uint64_ts */
    1:optional binary data,
    /* the number of values returned, or -1 on error */
    2:optional i32 count
}

/**************************************************************
 * Structure for Table Entry Modification                     *
 **************************************************************/

struct TableEntry {
    /* name of the rule entry */
    1: required string rule_name,
    /* flag indicating whether rule is the table default rule */
    2: optional bool default_rule,
    /* buffer containing match data in JSON format */
    3: optional binary match,
    /* buffer containing action data in JSON format */
    4: optional binary actions
    /* rule priority, ommited or <= 0 means lowest priority,
     * otherwise 1 is highest priority, 1++ is lower with 64k being lowest
     */
    5: optional i32 priority,
}

struct TableDesc {
    /* ID of table */
    1: required i32 tbl_id,
    /* name of table */
    2: optional string tbl_name,
    /* number of table entries:
     * note that the default rule is included in this count
     */
    3: optional i32 tbl_entries_max
}

/**************************************************************
 * Multicast Group Configuration                              *
 **************************************************************/

struct McastCfgEntry {
    /* name of table where entry is located */
    1: optional i32 group_id,
    /* maximum number of entries for the group */ 
    2: optional i32 max_entries,
    /* ports in multicast group */ 
    3: optional list <i32> ports
}

/**************************************************************
 * Port information                                            *
 **************************************************************/

struct PortInfo {
    /* Number to identify port */
    1: optional i32 id,
    /* Port token string */ 
    2: optional string token,
    /* Port doc */ 
    3: optional string info
}

/**************************************************************
 * Structure for Parser Value Sets                            *
 **************************************************************/

struct ParserValueSetFieldDesc {
    1:optional string name,
    2:optional i32 width,
}

struct ParserValueSetEntry {
    /* parser value set entry value */
    1: required string value,
    /* parser value set entry mask */
    2: optional string mask,
}

struct ParserValueSetDesc {
    /* ID of parser value set */
    1: required i32 pvs_id,
    /* name of parser value set */
    2: optional string pvs_name,
    /* maximum number of entries in set */
    3: optional i32 pvs_entries_max,
    /* key field components */
    4: optional list <ParserValueSetFieldDesc> key_layout,
}

/**************************************************************
 * Meters                                                     *
 **************************************************************/

enum MeterType {
    Invalid = 0,
    Global = 1,
    Direct = 2,
    Static = 3
}

enum MeterClass {
    Invalid = 0,
    Packets = 1,
    Bytes = 2
}

struct MeterCfg {
    1: optional double rate_k;
    2: optional i32 burst_k;
    3: optional i32 array_offset;
    4: optional i32 count;
}

struct MeterDesc {
    /* The meter name as defined in the P4 design */
    1:optional string name,
    /* A unique ID which is used to reference this counter */
    2:optional i32 id,
    /* The type of the counter */
    3:optional MeterType type,
    /* The type of the counter */
    4:optional MeterClass mclass,
    /* The size of the meter array:
     * In the case of Direct this is the size of the table
     * otherwise this is the the P4 instance count
     */
    5:optional i32 count,
    /* for Direct and Static, the table name
     * associated with the counter
     */
    6:optional string table,
    /* for Direct and Static, the table identifier
     * associated with the counter
     */
    7:optional i32 tableid,
}

/**************************************************************
 * Digests                                                    *
 **************************************************************/

struct DigestFieldDesc {
    1:optional string name,
    2:optional i32 width,
}

struct DigestDesc {
    /* The digest name as derived from the design */
    1:optional string name,
    /* A unique ID which is used to reference this digest */
    2:optional i32 id,
    /* The ID number that the application associated with the digest */
    3:optional i32 app_id,
    /* The field list name associated with the digest */
    4:optional string field_list_name,
    /* The fields in the digest, the digest payload will match this order */
    5:optional list <DigestFieldDesc> fields,
}

/**************************************************************
 * Opaque debug interface                                     *
 **************************************************************/

struct DebugCtlReturn {
    /* the debug return value, 0 on success, -1 on error */
    1:optional i32 return_value, 
    2:optional binary return_data,
}

/**************************************************************
 * Service                                                    *
 **************************************************************/

service RunTimeEnvironment
{
    /*
     *** System level functions ***
     */

    /* Return ping reply string to test connection */ 
    string sys_ping(),
    /* Print the echo_msg in the log (stdin, syslog or logfile) */
    void sys_echo(1:string echo_msg),
    /* Give shutdown command */
    oneway void sys_shutdown(),
    /* Retrieve RTE version string */
    string sys_version_get(),
    /* Retrieve current log level */
    LogLevel sys_log_level_get(),
    /* Set log level */
    RteReturn sys_log_level_set(1: LogLevel level),
    
    /*
     *** Design load, unload and configuration ***
     */

    /* Load design (unload before loading 2nd design) */
    RteReturn design_load(1:DesignLoadArgs arguments),
    /* Unload current design */
    RteReturn design_unload(),
    /* User configuration (P4 rule etc) load */ 
    RteReturn design_reconfig(1: binary pif_config_json),

    /*
     *** Hardware info and setup ***
     */

    /* Retrieve hardware port info */
    list <PortInfo> ports_info_retrieve(),

    /*
     *** Tables ***
     */

    /* List all the configured tables */
    list <TableDesc> table_list_all(),
    /* Add entry to user table (require all fields if it is a new entry) */
    RteReturn table_entry_add(1: i32 tbl_id, 2:TableEntry entry),
    /* Edit specified entry in user table 
     *(optional fields not being edited can be left out) */
    RteReturn table_entry_edit(1: i32 tbl_id, 2:TableEntry entry),
    /* Delete specified entry from user table (only required fields) */
    RteReturn table_entry_delete(1: i32 tbl_id, 2:TableEntry entry),
    /* List all entries in the table */
    list <TableEntry> table_retrieve(1: i32 tbl_id),
    /* Return a rules version number, useful for detecting external changes in
     * configuration, there is a single number for all tables; the value
     * increases in sequence and never wraps
     */
    i64 table_version_get(),
    
    /*
     *** Parser value sets ***
     */

    /* List parser value sets */
    list <ParserValueSetDesc> parser_value_set_list_all(),
    /* Add list of entries to specified parser value set */
    RteReturn parser_value_set_add(1: i32 pvs_id, 
                                   2: list <ParserValueSetEntry> entries),
    /* Clear entries of specified parser value set */
    RteReturn parser_value_set_clear(1: i32 pvs_id),
    /* List the entries in specified parser value set */
    list <ParserValueSetEntry> parser_value_set_retrieve(1: i32 pvs_id),

    /*
     *** P4 Counters ***
     */

    /* List all counters */
    list<P4CounterDesc> p4_counter_list_all(),
    /* Retrieve counter value */
    P4CounterReturn p4_counter_retrieve(1: i32 counter_id),
    /* Clear specified counter */
    RteReturn p4_counter_clear(1: i32 counter_id),
    /* Clear all counters */
    RteReturn p4_counter_clear_all(),

    /*
     *** Registers ***
     */

    /* List all registers */
    list <RegisterDesc> register_list_all(),
    /* Retrieve register values from a register array
     * Return consists of an array of string values corresponding to the 
     * fields for that register */
    list <string> register_retrieve(1: RegisterArrayArg regarr),
    /* Clear register array */
    RteReturn register_clear(1: RegisterArrayArg regarr),
    /* Set a field value for a register array */
    RteReturn register_field_set(1: RegisterArrayArg regarr,
                                 2: i32 field_id,
                                 3: string value),
    /* Set values for a register array 
     * Values must correspond the fields entry from the RegisterDesc */
    RteReturn register_set(1: RegisterArrayArg regarr,
                           2: list <string> values),

    /*
     *** System counters ***
     */

    /* Retrieve all system counters */ 
    list<SysCounterValue> sys_counter_retrieve_all(),
    /* Clear all system counters */
    RteReturn sys_counter_clear_all(),

    /*
     *** Multicast groups ***
     */

    /* Retrieve all the mcast configuration entries */
    list <McastCfgEntry> mcast_config_get_all(),
    /* Retrieve specified mcast configuration  entry */
    McastCfgEntry mcast_config_get(1: i32 mcast_group),
    /* Set mcast configuration */
    RteReturn mcast_config_set(1: McastCfgEntry cfg),

    /*
     *** Meters  ***
     */

    /* List all meters */
    list <MeterDesc> meter_list_all(),
    /* Retrieve specified meter configuration */
    list <MeterCfg> meter_config_get(1: i32 meter_id),
    /* Set specified meter configuration */
    RteReturn meter_config_set(1: i32 meter_id, 2: list <MeterCfg> cfgs),

    /*
     *** Digests ***
     */

    /* List all digests */
    list <DigestDesc> digest_list_all(),
    /* Register digestm returned regid used for other functions */
    i64 digest_register(1: i32 digest_id),
    /* Deregister digest with specified regid */
    RteReturn digest_deregister(1: i64 digest_regid),
    /* Retrieve digest with specified regid */
    list <string> digest_retrieve(1: i64 digest_regid),

    /*
     *** Opaque debug control and status ***
     */
    DebugCtlReturn debugctl(1: string debug_id, 2:binary debug_data)
}
