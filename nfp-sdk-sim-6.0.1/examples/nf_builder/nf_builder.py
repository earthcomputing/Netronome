#!/Library/Frameworks/Python.framework/Versions/3.4/bin/python3

import json
import sys

json_file = sys.argv[1]
if json_file is None:
    json_file = 'app.json'
    platfom_name = 'bataan'
else:

    platfom_name = sys.argv[2]
    if platfom_name is None:
        platfom_name = 'bataan'
    else:
        make_name = sys.argv[3]
        if make_name is None:
            make_name = "Makefile." + proj_name

f = open(json_file, 'r')
if f is None:
    print( "Can't open json file %s" % json_file )
    exit(1)
js = f.read()
jso = json.loads(js)
if jso is None:
    print( "Error reading json file" )
    exit(1)
proj_name = jso.get("name")
print(proj_name)
#proj_obj_name = proj_name + "_obj"

var_obj = jso.get("vars")
var_list = list(var_obj)
print(var_list)
#for v in var_list:
#    t = var_obj.get(v)
#    if isinstance(t, str):
#        print( v + ": " + t )
#    else:
#        print( "%s: %d" % (v, t))



mfile = open( make_name, 'w')
if mfile is None:
    print( "Can't open output file %s" % make_name )
    exit(1)

comment = """
#######################################################################################################################
# Auto generated by %s
#   Do not edit manually!! 
#######################################################################################################################
"""
mfile.write( comment % sys.argv[0] )
mfile.write( "include Makefile.head\n")
#print(make_name)

common_obj = jso.get('common')
common_comple_flags = ""
common_comp_flags = common_obj.get('compiler').get('flags')
for frg in common_comp_flags:
    common_comple_flags += " " + frg

common_link_flags = ""
linker_obj = common_obj.get('linker')
if not linker_obj is None:
    common_linker_flags = linker_obj.get('flags')
    if not common_linker_flags is None:
        for f in common_linker_flags:
            common_link_flags += ( f + " ")
    pico_obj = linker_obj.get( "pico")

#print(common_comple_flags)

#component
components_obj = jso.get('components')
components_list = list(components_obj)
valid_compo_list = []
for compo in components_list:
    comp_obj = components_obj.get( compo )
    if not comp_obj is None:
        compiler_obj = comp_obj.get( "compiler" )
        if not compiler_obj is None:
            src_list = comp_obj
            if not src_list is None:
                valid_compo_list.append( compo )
print ( valid_compo_list )
targets_obj = jso.get

platforms_obj = jso.get( 'platforms' )

tgets_obj = jso.get( "targets" )

targets_list = list(tgets_obj)


def var_convert( s ):
    import re
    p = re.compile( '%\((\S+)\)s' )
    result = p.search(s)
    if( result is None ):
        return s
    else:
        nn = result.group(1)
        #print( "found!" + nn )
        for v in var_list:
            if nn == v:
                t = var_obj.get(v)
                if not isinstance(t, str):
                    t = str(t)
                re = p.sub( t, s )
                print( re )
                return re
            #else:
                #print ( nn + " is not " + v )
    return s

def gen_compo( name, compo_name, last ):
    print( "platform %s component:%s" %(name, compo_name))
    compo_obj = components_obj.get( compo_name )
    if compo_obj is None:
        return
    comp_o = compo_obj.get("compiler")
    if not comp_o is None:
        inc_list = comp_o.get( "include" )
        if not inc_list is None:
            for inc in inc_list:
                inc = var_convert(inc)
                mfile.write( "$(eval $(call microc.add_include,%s,%s ))" % (name, inc))
        dir = compo_obj.get( "dir" )
        if dir is None:
            dir = "."
        else:
            dir = var_convert( dir )
        if dir == "":
            dir = "."
        src_list = comp_o.get("src")
        if not src_list is None:
            length = len(src_list)-1
            index = 0
            for src in src_list:
                src = var_convert(src)
                #src = src.split('.', 1)[0]
                if last is True and index == length:
                    #print( "$(eval $(call micro_c.compile_with_rtl, %s, %s, %s))" %(list_name, src, dir))
                    mfile.write( "$(eval $(call micro_c.compile_with_rtl,%s,%s.c,%s))\n" %(name, src, dir))
                else:
                    #print( "$(eval $(call micro_c.add_src_lib, %s, %s, %s))" %(list_name, src, dir))
                    mfile.write("$(eval $(call micro_c.add_src_lib,%s,%s,%s))\n" % (name, src, dir))
                index += 1


## Compile target per platform level to refrect the flags per platform level
#  name carries the 
def gen_target( name, tgt_obj ):
    compo_list = tgt_obj.get("components")
    comp_flags = ""
    comp_def = ""
    comp_obj = tgt_obj.get( "compiler" )
    if not comp_obj is None:
        flag_list = comp_obj.get("flags")
        if not flag_list is None:
            for f in flag_list:
                f = var_convert( f )
                #print( "$(eval $(microc.add_flags( %s, %s ))" % (l_name, f))
                mfile.write("$(eval $(call microc.add_flags, %s, %s ))\n" % (name, f))
        def_list = comp_obj.get("defines")
        if not def_list is None:
            for d in def_list:
                d = var_convert( d )
                #print( "$(eval $(microc.add_define( %s, %s ))" % (l_name, d))
                mfile.write( "$(eval $(call microc.add_define, %s, %s ))\n" % (name, d))
    length = 0
    for compo_name in compo_list:
        if compo_name in valid_compo_list:
            length += 1
    index = 0
    for compo_name in compo_list:
        if compo_name in valid_compo_list:
            #print( "index %d length %d" % (index, length))
            #mfile.write( "\n")
            gen_compo( name, compo_name, index == length-1 )
            index += 1
    # mes in target is not supported for now
    #mes = tgt_obj.get("mes")
    #if not mes is None:
    #    for me in mes:
    #        me = var_convert( me )
    #        #print('$(eval $(call fw.add_obj, %s, %s, %s))' % (proj_obj_name, l_name, me))
    #        mfile.write('$(eval $(call fw.add_obj,%s,%s,%s))\n' % (proj_obj_name, l_name, me))

def gen_platform( platform, p_obj ):
    # process "common" : "defines"
    common_obj = p_obj.get( "common" )
    if not common_obj is None:
        common_defs = common_obj.get("defines")
        #if not defs is None:
        #    for def in defs:
        #        mfile.write( "$(eval $(microc.add_define( %s,%s ))\n" % (name, def))
    targets_obj = p_obj.get( "targets" )
    t_list = list( targets_obj )
    for target in t_list:
        targ_obj = targets_obj.get( target )
        platform_target_name = platform + "_" + target
        mes_list = targ_obj.get( "mes" )
        if not mes_list is None:
            #process common defines
            if not common_defs is None:
                for d in common_defs:
                    mfile.write( "$(eval $(call microc.add_define, %s, %s ))\n" % (platform_target_name, d))
            # process per target defines 
            compiler_obj = targ_obj.get("compiler")
            if not compiler_obj is None:
                defines_array = compiler_obj.get("defines")
                if not defines_array is None:
                    for d in defines_array:
                        mfile.write( "$(eval $(call microc.add_define, %s, %s ))\n" % (platform_target_name, d))
                includes_array = compiler_obj.get("include")
                if not includes_array is None:
                    for inc in includes_array:
                        inc = var_convert(inc)
                        mfile.write("$(eval $(call microc.add_include,%s,%s ))" % (platform_target_name, inc))
            tgt_obj = tgets_obj.get(target)
            gen_target( platform_target_name, tgt_obj )
            for me in mes_list:
                me = var_convert( me )
                #print( '$(eval $(call fw.add_obj,%s,%s,%s))' % ( proj_obj_name, target, me ))
                mfile.write('$(eval $(call fw.add_obj,%s,%s,%s))\n' % (proj_name, platform_target_name, me))

#testing gen_target
#for tgt in targets_list:
    #if( tgt == "pif_app_nfd" ):
    #tgt_obj = tgets_obj.get( tgt )
    #gen_target( tgt, tgt_obj )


if platforms_obj is None:
    exit(0)

platforms_list = list( platforms_obj )
for platform in platforms_list:
    if platform == platfom_name:
        plf_obj = platforms_obj.get(platform)
        gen_platform( platform, plf_obj )

        mfile.write("$(eval $(call fw.add_ppc,%s,i8,$(PICO_CODE)))\n" % proj_name )
        mfile.write("$(eval $(call fw.add_ppc,%s,i9,$(PICO_CODE)))\n" % proj_name )
        mfile.write("$(eval $(call fw.link_with_rtsyms,%s))\n" % proj_name )

""" testing var_convert
compos = jso.get('components')
pif_m = compos.get( 'pif_main_common')
comp = pif_m.get( 'compiler')
src = comp.get('src')
for st in src:
    print(st)
    var_convert(st)
"""


mfile.write( "include Makefile.tail\n")



