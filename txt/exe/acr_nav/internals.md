## acr_nav - Internals


### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_nav/content.cpp](/cpp/acr_nav/content.cpp)||
|[cpp/acr_nav/graph.cpp](/cpp/acr_nav/graph.cpp)||
|[cpp/acr_nav/main.cpp](/cpp/acr_nav/main.cpp)||
|[cpp/acr_nav/nav.cpp](/cpp/acr_nav/nav.cpp)||
|[cpp/acr_nav/render.cpp](/cpp/acr_nav/render.cpp)||
|[cpp/acr_nav/util.cpp](/cpp/acr_nav/util.cpp)||
|[cpp/gen/acr_nav_gen.cpp](/cpp/gen/acr_nav_gen.cpp)||
|[include/acr_nav.h](/include/acr_nav.h)||
|[include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)||
|[include/gen/acr_nav_gen.inl.h](/include/gen/acr_nav_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`acr_nav` generated code creates the tables below.
All allocations are done through global `acr_nav::_db` [acr_nav.FDb](#acr_nav-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[acr_nav.Ack](#acr_nav-ack)||
|[acr_nav.ContentRow](#acr_nav-contentrow)||FViewmode.content_row (Tary)|
|[acr_nav.FCtype](#acr_nav-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|
||||FField.p_ctype (Upptr)|
||||FField.p_arg (Upptr)|
||||FNs.c_ctype (Ptrary)|
||||FSsimfile.p_ctype (Upptr)|
||||FViewmode.ensure_content (Hook)|
|[acr_nav.FDb](#acr_nav-fdb)||FDb._db (Global)|
|[acr_nav.FDetailsrc](#acr_nav-fdetailsrc)|[acr_navdb.detailsrc](/txt/ssimdb/acr_navdb/detailsrc.md)|FDb.detailsrc (Lary)|detailsrc (Lary, by rowid)|ind_detailsrc (Thash, hash field detailsrc)|
|[acr_nav.FField](#acr_nav-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|p_detail_field (Ptr)|
||||FCtype.c_field (Ptrary)|
||||FCtype.c_field_arg (Ptrary)|
|[acr_nav.FFiltertarget](#acr_nav-ffiltertarget)|[acr_navdb.filtertarget](/txt/ssimdb/acr_navdb/filtertarget.md)|FDb.filtertarget (Lary)|filtertarget (Lary, by rowid)|ind_filtertarget (Thash, hash field filtertarget)|p_cur_filtertarget (Ptr)|p_default_filtertarget (Ptr)|
|[acr_nav.FHelpgroup](#acr_nav-fhelpgroup)|[acr_navdb.helpgroup](/txt/ssimdb/acr_navdb/helpgroup.md)|FDb.helpgroup (Lary)|helpgroup (Lary, by rowid)|ind_helpgroup (Thash, hash field helpgroup)|
||||FNavaction.p_helpgroup (Ptr)|
|[acr_nav.FKeybind](#acr_nav-fkeybind)|[acr_navdb.keybind](/txt/ssimdb/acr_navdb/keybind.md)|FDb.keybind (Lary)|keybind (Lary, by rowid)|ind_keybind (Thash, hash field keybind)|
|[acr_nav.FNavaction](#acr_nav-fnavaction)|[acr_navdb.navaction](/txt/ssimdb/acr_navdb/navaction.md)|FDb.navaction (Lary)|**static**|navaction (Lary, by rowid)|ind_navaction (Thash, hash field navaction)|
||||FKeybind.p_navaction (Upptr)|
|[acr_nav.FNavmode](#acr_nav-fnavmode)|[acr_navdb.navmode](/txt/ssimdb/acr_navdb/navmode.md)|FDb.navmode (Lary)|navmode (Lary, by rowid)|ind_navmode (Thash, hash field navmode)|p_cur_mode (Ptr)|p_filter_mode (Ptr)|
|[acr_nav.FNavstyle](#acr_nav-fnavstyle)|[acr_navdb.navstyle](/txt/ssimdb/acr_navdb/navstyle.md)|FDb.navstyle (Lary)|navstyle (Lary, by rowid)|ind_navstyle (Thash, hash field navstyle)|
||||FReftypestyle.p_navstyle (Upptr)|
||||LineColorSpan.p_navstyle (Upptr)|
|[acr_nav.FNs](#acr_nav-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|p_nsdep_ns (Ptr)|
||||FCtype.p_ns (Upptr)|
|[acr_nav.FPanel](#acr_nav-fpanel)|[acr_navdb.panel](/txt/ssimdb/acr_navdb/panel.md)|FDb.panel (Lary)|panel (Lary, by rowid)|ind_panel (Thash, hash field panel)|p_cur_panel (Ptr)|p_left_panel (Ptr)|p_right_panel (Ptr)|
|[acr_nav.FReftype](#acr_nav-freftype)|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|FDb.reftype (Lary)|reftype (Lary, by rowid)|ind_reftype (Thash, hash field reftype)|
||||FField.p_reftype (Upptr)|
|[acr_nav.FReftypestyle](#acr_nav-freftypestyle)|[acr_navdb.reftypestyle](/txt/ssimdb/acr_navdb/reftypestyle.md)|FDb.reftypestyle (Lary)|reftypestyle (Lary, by rowid)|ind_reftypestyle (Thash, hash field reftypestyle)|
||||FReftype.c_reftypestyle (Ptr)|
|[acr_nav.FSsimfile](#acr_nav-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
|[acr_nav.FViewmode](#acr_nav-fviewmode)|[acr_navdb.viewmode](/txt/ssimdb/acr_navdb/viewmode.md)|FDb.viewmode (Lary)|**static**|viewmode (Lary, by rowid)|ind_viewmode (Thash, hash field viewmode)|p_cur_viewmode (Ptr)|p_default_viewmode (Ptr)|p_pre_nsdep_viewmode (Ptr)|
|[acr_nav.GoBack](#acr_nav-goback)||
|[acr_nav.InputError](#acr_nav-inputerror)||
|[acr_nav.LeftItem](#acr_nav-leftitem)||FDb.left_item (Tary)|
|[acr_nav.LineColorSpan](#acr_nav-linecolorspan)||FViewmode.cspan (Tary)|
|[acr_nav.Naventry](#acr_nav-naventry)||FDb.navstack (Tary)|
|[acr_nav.Navigate](#acr_nav-navigate)||
|[acr_nav.OverlayEntry](#acr_nav-overlayentry)||FDb.overlay_stack (Tary)|
|[acr_nav.PanelState](#acr_nav-panelstate)||
|[acr_nav.PreviewNavCol](#acr_nav-previewnavcol)||FViewmode.nav_col (Tary)|
|[acr_nav.Screen](#acr_nav-screen)||
|[acr_nav.Screenshot](#acr_nav-screenshot)||
|[acr_nav.SendKey](#acr_nav-sendkey)||
|[acr_nav.SetFilter](#acr_nav-setfilter)||
|[acr_nav.SetTermSize](#acr_nav-settermsize)||
|[acr_nav.SetView](#acr_nav-setview)||
|[acr_nav.Summary](#acr_nav-summary)||
|[acr_nav.VisibleField](#acr_nav-visiblefield)||
|[acr_nav.VisibleLeftItem](#acr_nav-visibleleftitem)||
|[acr_nav.VisibleLine](#acr_nav-visibleline)||

#### acr_nav.Ack - Headless output: command acknowledgment
<a href="#acr_nav-ack"></a>

#### acr_nav.Ack Fields
<a href="#acr_nav-ack-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Ack.ack|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Command type tag|
|acr_nav.Ack.ok|bool|[Val](/txt/exe/amc/reftypes.md#val)||Success|
|acr_nav.Ack.ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Selected ctype|
|acr_nav.Ack.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Current viewmode|
|acr_nav.Ack.navstack_depth|i32|[Val](/txt/exe/amc/reftypes.md#val)||Navstack depth|
|acr_nav.Ack.msg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Error message|

#### Struct Ack
<a href="#struct-ack"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Ack { // acr_nav.Ack: Headless output: command acknowledgment
    algo::Smallstr50    ack;              // Command type tag
    bool                ok;               //   false  Success
    algo::Smallstr100   ctype;            // Selected ctype
    algo::Smallstr50    viewmode;         // Current viewmode
    i32                 navstack_depth;   //   0  Navstack depth
    algo::cstring       msg;              // Error message
    // func:acr_nav.Ack..Ctor
    inline               Ack() __attribute__((nothrow));
};
```

#### acr_nav.ContentRow - Content row with display text and per-column navigation targets
<a href="#acr_nav-contentrow"></a>

#### acr_nav.ContentRow Fields
<a href="#acr_nav-contentrow-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.ContentRow.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Formatted display line|
|acr_nav.ContentRow.nav_target|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Inlary](/txt/exe/amc/reftypes.md#inlary)||Navigation target ctype key per column (empty=not navigable)|

#### Struct ContentRow
<a href="#struct-contentrow"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct ContentRow { // acr_nav.ContentRow: Content row with display text and per-column navigation targets
    algo::cstring   text;                                             // Formatted display line
    u8              nav_target_data[sizeof(algo::Smallstr100) * 4];   // place for data
    i32             nav_target_n;                                     // number of elems current in existence
    enum { nav_target_max = 4 };
    // func:acr_nav.ContentRow..AssignOp
    inline acr_nav::ContentRow& operator =(const acr_nav::ContentRow &rhs) __attribute__((nothrow));
    // func:acr_nav.ContentRow..Ctor
    inline               ContentRow() __attribute__((nothrow));
    // func:acr_nav.ContentRow..Dtor
    inline               ~ContentRow() __attribute__((nothrow));
    // func:acr_nav.ContentRow..CopyCtor
    inline               ContentRow(const acr_nav::ContentRow &rhs) __attribute__((nothrow));
};
```

#### acr_nav.FCtype - Struct
<a href="#acr_nav-fctype"></a>

#### acr_nav.FCtype Fields
<a href="#acr_nav-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FCtype.base|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|acr_nav.FCtype.c_field|[acr_nav.FField](/txt/exe/acr_nav/internals.md#acr_nav-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr_nav.FCtype.c_field_arg|[acr_nav.FField](/txt/exe/acr_nav/internals.md#acr_nav-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Fields from other ctypes whose arg is this ctype (reverse xrefs)|
|acr_nav.FCtype.c_ssimfile|[acr_nav.FSsimfile](/txt/exe/acr_nav/internals.md#acr_nav-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_nav.FCtype.p_ns|[acr_nav.FNs](/txt/exe/acr_nav/internals.md#acr_nav-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``acr_nav.FCtype.base`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FCtype { // acr_nav.FCtype
    algo::Smallstr100     ctype;               // Identifier. must be ns.typename
    algo::Comment         comment;             //
    acr_nav::FField**     c_field_elems;       // array of pointers
    u32                   c_field_n;           // array of pointers
    u32                   c_field_max;         // capacity of allocated array
    acr_nav::FField**     c_field_arg_elems;   // array of pointers
    u32                   c_field_arg_n;       // array of pointers
    u32                   c_field_arg_max;     // capacity of allocated array
    acr_nav::FSsimfile*   c_ssimfile;          // optional pointer
    acr_nav::FNs*         p_ns;                // reference to parent row
    bool                  ns_c_ctype_in_ary;   //   false  membership flag
    acr_nav::FCtype*      ind_ctype_next;      // hash next
    u32                   ind_ctype_hashval;   // hash value
    // reftype Ptrary of acr_nav.FCtype.c_field prohibits copy
    // reftype Ptrary of acr_nav.FCtype.c_field_arg prohibits copy
    // x-reference on acr_nav.FCtype.c_ssimfile prevents copy
    // x-reference on acr_nav.FCtype.p_ns prevents copy
    // func:acr_nav.FCtype..AssignOp
    acr_nav::FCtype&     operator =(const acr_nav::FCtype &rhs) = delete;
    // reftype Ptrary of acr_nav.FCtype.c_field prohibits copy
    // reftype Ptrary of acr_nav.FCtype.c_field_arg prohibits copy
    // x-reference on acr_nav.FCtype.c_ssimfile prevents copy
    // x-reference on acr_nav.FCtype.p_ns prevents copy
    // func:acr_nav.FCtype..CopyCtor
    FCtype(const acr_nav::FCtype &rhs) = delete;
private:
    // func:acr_nav.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:acr_nav.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend acr_nav::FCtype&     ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FCtype*     ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveAll() __attribute__((nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FDb - 
<a href="#acr_nav-fdb"></a>

#### acr_nav.FDb Fields
<a href="#acr_nav-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FDb._db|[acr_nav.FDb](/txt/exe/acr_nav/internals.md#acr_nav-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|acr_nav.FDb.cmdline|[command.acr_nav](/txt/protocol/command/README.md#command-acr_nav)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.FDb.ctype|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.detailsrc|[acr_nav.FDetailsrc](/txt/exe/acr_nav/internals.md#acr_nav-fdetailsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.helpgroup|[acr_nav.FHelpgroup](/txt/exe/acr_nav/internals.md#acr_nav-fhelpgroup)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_helpgroup|[acr_nav.FHelpgroup](/txt/exe/acr_nav/internals.md#acr_nav-fhelpgroup)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.ind_ctype|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.ind_detailsrc|[acr_nav.FDetailsrc](/txt/exe/acr_nav/internals.md#acr_nav-fdetailsrc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.field|[acr_nav.FField](/txt/exe/acr_nav/internals.md#acr_nav-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_field|[acr_nav.FField](/txt/exe/acr_nav/internals.md#acr_nav-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.ns|[acr_nav.FNs](/txt/exe/acr_nav/internals.md#acr_nav-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_ns|[acr_nav.FNs](/txt/exe/acr_nav/internals.md#acr_nav-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.reftype|[acr_nav.FReftype](/txt/exe/acr_nav/internals.md#acr_nav-freftype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_reftype|[acr_nav.FReftype](/txt/exe/acr_nav/internals.md#acr_nav-freftype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.navaction|[acr_nav.FNavaction](/txt/exe/acr_nav/internals.md#acr_nav-fnavaction)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_navaction|[acr_nav.FNavaction](/txt/exe/acr_nav/internals.md#acr_nav-fnavaction)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.keybind|[acr_nav.FKeybind](/txt/exe/acr_nav/internals.md#acr_nav-fkeybind)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_keybind|[acr_nav.FKeybind](/txt/exe/acr_nav/internals.md#acr_nav-fkeybind)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.panel|[acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_panel|[acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.navmode|[acr_nav.FNavmode](/txt/exe/acr_nav/internals.md#acr_nav-fnavmode)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_navmode|[acr_nav.FNavmode](/txt/exe/acr_nav/internals.md#acr_nav-fnavmode)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.navstack|[acr_nav.Naventry](/txt/exe/acr_nav/internals.md#acr_nav-naventry)|[Tary](/txt/exe/amc/reftypes.md#tary)||Navigation history|
|acr_nav.FDb.p_cur_panel|[acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Currently focused panel|
|acr_nav.FDb.p_left_panel|[acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Left panel (ctype list). Named ptr is deliberate: 2-panel layout is fixed, position-based lookup unnecessary|
|acr_nav.FDb.p_right_panel|[acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Right panel (content). Named ptr is deliberate: 2-panel layout is fixed, position-based lookup unnecessary|
|acr_nav.FDb.p_cur_mode|[acr_nav.FNavmode](/txt/exe/acr_nav/internals.md#acr_nav-fnavmode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Current UI mode (browse/filter)|
|acr_nav.FDb.p_filter_mode|[acr_nav.FNavmode](/txt/exe/acr_nav/internals.md#acr_nav-fnavmode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Cached pointer to filter navmode for fast comparison|
|acr_nav.FDb.filter|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Current filter text|
|acr_nav.FDb.running|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Event loop control|
|acr_nav.FDb.term_hei|i32|[Val](/txt/exe/amc/reftypes.md#val)|40|Terminal height|
|acr_nav.FDb.term_wid|i32|[Val](/txt/exe/amc/reftypes.md#val)|120|Terminal width|
|acr_nav.FDb.navstyle|[acr_nav.FNavstyle](/txt/exe/acr_nav/internals.md#acr_nav-fnavstyle)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_navstyle|[acr_nav.FNavstyle](/txt/exe/acr_nav/internals.md#acr_nav-fnavstyle)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.reftypestyle|[acr_nav.FReftypestyle](/txt/exe/acr_nav/internals.md#acr_nav-freftypestyle)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_reftypestyle|[acr_nav.FReftypestyle](/txt/exe/acr_nav/internals.md#acr_nav-freftypestyle)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.startup_help|bool|[Val](/txt/exe/amc/reftypes.md#val)||True on startup; first keypress dismisses help|
|acr_nav.FDb.ssimfile|[acr_nav.FSsimfile](/txt/exe/acr_nav/internals.md#acr_nav-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_ssimfile|[acr_nav.FSsimfile](/txt/exe/acr_nav/internals.md#acr_nav-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.viewmode|[acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_viewmode|[acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.p_cur_viewmode|[acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Current right-panel view mode|
|acr_nav.FDb.p_default_viewmode|[acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Default viewmode (viewmode:fields)|
|acr_nav.FDb.p_detail_field|[acr_nav.FField](/txt/exe/acr_nav/internals.md#acr_nav-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Field being detailed (non-null in detail mode)|
|acr_nav.FDb.left_item|[acr_nav.LeftItem](/txt/exe/acr_nav/internals.md#acr_nav-leftitem)|[Tary](/txt/exe/amc/reftypes.md#tary)||Display list for left panel|
|acr_nav.FDb.n_visible_ctype|i32|[Val](/txt/exe/amc/reftypes.md#val)||Count of visible ctypes (excludes ns headers)|
|acr_nav.FDb.filtertarget|[acr_nav.FFiltertarget](/txt/exe/acr_nav/internals.md#acr_nav-ffiltertarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_nav.FDb.ind_filtertarget|[acr_nav.FFiltertarget](/txt/exe/acr_nav/internals.md#acr_nav-ffiltertarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_nav.FDb.p_cur_filtertarget|[acr_nav.FFiltertarget](/txt/exe/acr_nav/internals.md#acr_nav-ffiltertarget)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Current filter target|
|acr_nav.FDb.p_default_filtertarget|[acr_nav.FFiltertarget](/txt/exe/acr_nav/internals.md#acr_nav-ffiltertarget)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Cached pointer to ctype filtertarget|
|acr_nav.FDb.filter_regx|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Val](/txt/exe/amc/reftypes.md#val)||Cached compiled filter regex|
|acr_nav.FDb.pre_filter_text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Saved filter text before entering filter mode|
|acr_nav.FDb.pre_filter_target|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Saved filtertarget before entering filter mode|
|acr_nav.FDb.pre_filter_sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Saved left panel sel_row before entering filter mode|
|acr_nav.FDb.pre_filter_scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)||Saved left panel scroll_offset before entering filter mode|
|acr_nav.FDb.p_nsdep_ns|[acr_nav.FNs](/txt/exe/acr_nav/internals.md#acr_nav-fns)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Namespace whose deps are currently cached in nsdep view|
|acr_nav.FDb.sel_nav_col|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Selected navigable column index (into preview_nav)|
|acr_nav.FDb.preview_nav_pending|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Pending pkey match after preview follow-ref navigation|
|acr_nav.FDb.sel_nav_col_pending|i32|[Val](/txt/exe/amc/reftypes.md#val)|-1|Pending sel_nav_col for navstack restore (-1 = none)|
|acr_nav.FDb.p_pre_nsdep_viewmode|[acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Viewmode saved before nsdep context switch|
|acr_nav.FDb.overlay_stack|[acr_nav.OverlayEntry](/txt/exe/acr_nav/internals.md#acr_nav-overlayentry)|[Tary](/txt/exe/amc/reftypes.md#tary)||Overlay viewmode save/restore stack|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FDb { // acr_nav.FDb
    command::acr_nav           cmdline;                          //
    acr_nav::FCtype*           ctype_lary[32];                   // level array
    i32                        ctype_n;                          // number of elements in array
    acr_nav::FDetailsrc*       detailsrc_lary[32];               // level array
    i32                        detailsrc_n;                      // number of elements in array
    acr_nav::FHelpgroup*       helpgroup_lary[32];               // level array
    i32                        helpgroup_n;                      // number of elements in array
    acr_nav::FHelpgroup**      ind_helpgroup_buckets_elems;      // pointer to bucket array
    i32                        ind_helpgroup_buckets_n;          // number of elements in bucket array
    i32                        ind_helpgroup_n;                  // number of elements in the hash table
    acr_nav::FCtype**          ind_ctype_buckets_elems;          // pointer to bucket array
    i32                        ind_ctype_buckets_n;              // number of elements in bucket array
    i32                        ind_ctype_n;                      // number of elements in the hash table
    acr_nav::FDetailsrc**      ind_detailsrc_buckets_elems;      // pointer to bucket array
    i32                        ind_detailsrc_buckets_n;          // number of elements in bucket array
    i32                        ind_detailsrc_n;                  // number of elements in the hash table
    acr_nav::FField*           field_lary[32];                   // level array
    i32                        field_n;                          // number of elements in array
    acr_nav::FField**          ind_field_buckets_elems;          // pointer to bucket array
    i32                        ind_field_buckets_n;              // number of elements in bucket array
    i32                        ind_field_n;                      // number of elements in the hash table
    acr_nav::FNs*              ns_lary[32];                      // level array
    i32                        ns_n;                             // number of elements in array
    acr_nav::FNs**             ind_ns_buckets_elems;             // pointer to bucket array
    i32                        ind_ns_buckets_n;                 // number of elements in bucket array
    i32                        ind_ns_n;                         // number of elements in the hash table
    acr_nav::FReftype*         reftype_lary[32];                 // level array
    i32                        reftype_n;                        // number of elements in array
    acr_nav::FReftype**        ind_reftype_buckets_elems;        // pointer to bucket array
    i32                        ind_reftype_buckets_n;            // number of elements in bucket array
    i32                        ind_reftype_n;                    // number of elements in the hash table
    acr_nav::FNavaction*       navaction_lary[32];               // level array
    i32                        navaction_n;                      // number of elements in array
    acr_nav::FNavaction**      ind_navaction_buckets_elems;      // pointer to bucket array
    i32                        ind_navaction_buckets_n;          // number of elements in bucket array
    i32                        ind_navaction_n;                  // number of elements in the hash table
    acr_nav::FKeybind*         keybind_lary[32];                 // level array
    i32                        keybind_n;                        // number of elements in array
    acr_nav::FKeybind**        ind_keybind_buckets_elems;        // pointer to bucket array
    i32                        ind_keybind_buckets_n;            // number of elements in bucket array
    i32                        ind_keybind_n;                    // number of elements in the hash table
    acr_nav::FPanel*           panel_lary[32];                   // level array
    i32                        panel_n;                          // number of elements in array
    acr_nav::FPanel**          ind_panel_buckets_elems;          // pointer to bucket array
    i32                        ind_panel_buckets_n;              // number of elements in bucket array
    i32                        ind_panel_n;                      // number of elements in the hash table
    acr_nav::FNavmode*         navmode_lary[32];                 // level array
    i32                        navmode_n;                        // number of elements in array
    acr_nav::FNavmode**        ind_navmode_buckets_elems;        // pointer to bucket array
    i32                        ind_navmode_buckets_n;            // number of elements in bucket array
    i32                        ind_navmode_n;                    // number of elements in the hash table
    acr_nav::Naventry*         navstack_elems;                   // pointer to elements
    u32                        navstack_n;                       // number of elements in array
    u32                        navstack_max;                     // max. capacity of array before realloc
    acr_nav::FPanel*           p_cur_panel;                      // Currently focused panel. optional pointer
    acr_nav::FPanel*           p_left_panel;                     // Left panel (ctype list). Named ptr is deliberate: 2-panel layout is fixed, position-based lookup unnecessary. optional pointer
    acr_nav::FPanel*           p_right_panel;                    // Right panel (content). Named ptr is deliberate: 2-panel layout is fixed, position-based lookup unnecessary. optional pointer
    acr_nav::FNavmode*         p_cur_mode;                       // Current UI mode (browse/filter). optional pointer
    acr_nav::FNavmode*         p_filter_mode;                    // Cached pointer to filter navmode for fast comparison. optional pointer
    algo::cstring              filter;                           // Current filter text
    bool                       running;                          //   true  Event loop control
    i32                        term_hei;                         //   40  Terminal height
    i32                        term_wid;                         //   120  Terminal width
    acr_nav::FNavstyle*        navstyle_lary[32];                // level array
    i32                        navstyle_n;                       // number of elements in array
    acr_nav::FNavstyle**       ind_navstyle_buckets_elems;       // pointer to bucket array
    i32                        ind_navstyle_buckets_n;           // number of elements in bucket array
    i32                        ind_navstyle_n;                   // number of elements in the hash table
    acr_nav::FReftypestyle*    reftypestyle_lary[32];            // level array
    i32                        reftypestyle_n;                   // number of elements in array
    acr_nav::FReftypestyle**   ind_reftypestyle_buckets_elems;   // pointer to bucket array
    i32                        ind_reftypestyle_buckets_n;       // number of elements in bucket array
    i32                        ind_reftypestyle_n;               // number of elements in the hash table
    bool                       startup_help;                     //   false  True on startup; first keypress dismisses help
    acr_nav::FSsimfile*        ssimfile_lary[32];                // level array
    i32                        ssimfile_n;                       // number of elements in array
    acr_nav::FSsimfile**       ind_ssimfile_buckets_elems;       // pointer to bucket array
    i32                        ind_ssimfile_buckets_n;           // number of elements in bucket array
    i32                        ind_ssimfile_n;                   // number of elements in the hash table
    acr_nav::FViewmode*        viewmode_lary[32];                // level array
    i32                        viewmode_n;                       // number of elements in array
    acr_nav::FViewmode**       ind_viewmode_buckets_elems;       // pointer to bucket array
    i32                        ind_viewmode_buckets_n;           // number of elements in bucket array
    i32                        ind_viewmode_n;                   // number of elements in the hash table
    acr_nav::FViewmode*        p_cur_viewmode;                   // Current right-panel view mode. optional pointer
    acr_nav::FViewmode*        p_default_viewmode;               // Default viewmode (viewmode:fields). optional pointer
    acr_nav::FField*           p_detail_field;                   // Field being detailed (non-null in detail mode). optional pointer
    acr_nav::LeftItem*         left_item_elems;                  // pointer to elements
    u32                        left_item_n;                      // number of elements in array
    u32                        left_item_max;                    // max. capacity of array before realloc
    i32                        n_visible_ctype;                  //   0  Count of visible ctypes (excludes ns headers)
    acr_nav::FFiltertarget*    filtertarget_lary[32];            // level array
    i32                        filtertarget_n;                   // number of elements in array
    acr_nav::FFiltertarget**   ind_filtertarget_buckets_elems;   // pointer to bucket array
    i32                        ind_filtertarget_buckets_n;       // number of elements in bucket array
    i32                        ind_filtertarget_n;               // number of elements in the hash table
    acr_nav::FFiltertarget*    p_cur_filtertarget;               // Current filter target. optional pointer
    acr_nav::FFiltertarget*    p_default_filtertarget;           // Cached pointer to ctype filtertarget. optional pointer
    algo_lib::Regx             filter_regx;                      // Cached compiled filter regex
    algo::cstring              pre_filter_text;                  // Saved filter text before entering filter mode
    algo::Smallstr50           pre_filter_target;                // Saved filtertarget before entering filter mode
    i32                        pre_filter_sel_row;               //   0  Saved left panel sel_row before entering filter mode
    i32                        pre_filter_scroll_offset;         //   0  Saved left panel scroll_offset before entering filter mode
    acr_nav::FNs*              p_nsdep_ns;                       // Namespace whose deps are currently cached in nsdep view. optional pointer
    i32                        sel_nav_col;                      //   0  Selected navigable column index (into preview_nav)
    algo::cstring              preview_nav_pending;              // Pending pkey match after preview follow-ref navigation
    i32                        sel_nav_col_pending;              //   -1  Pending sel_nav_col for navstack restore (-1 = none)
    acr_nav::FViewmode*        p_pre_nsdep_viewmode;             // Viewmode saved before nsdep context switch. optional pointer
    acr_nav::OverlayEntry*     overlay_stack_elems;              // pointer to elements
    u32                        overlay_stack_n;                  // number of elements in array
    u32                        overlay_stack_max;                // max. capacity of array before realloc
    acr_nav::trace             trace;                            //
};
```

#### acr_nav.FDetailsrc - Ssimfile to scan for field detail metadata
<a href="#acr_nav-fdetailsrc"></a>

#### acr_nav.FDetailsrc Fields
<a href="#acr_nav-fdetailsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FDetailsrc.base|[acr_navdb.Detailsrc](/txt/ssimdb/acr_navdb/detailsrc.md)|[Base](/txt/ssimdb/acr_navdb/detailsrc.md)|||

#### Struct FDetailsrc
<a href="#struct-fdetailsrc"></a>
*Note:* field ``acr_nav.FDetailsrc.base`` has reftype ``base`` so the fields of [acr_navdb.Detailsrc](/txt/ssimdb/acr_navdb/detailsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FDetailsrc { // acr_nav.FDetailsrc
    acr_nav::FDetailsrc*   ind_detailsrc_next;      // hash next
    u32                    ind_detailsrc_hashval;   // hash value
    algo::Smallstr50       detailsrc;               //
    algo::Comment          comment;                 //
    // func:acr_nav.FDetailsrc..AssignOp
    inline acr_nav::FDetailsrc& operator =(const acr_nav::FDetailsrc &rhs) = delete;
    // func:acr_nav.FDetailsrc..CopyCtor
    inline               FDetailsrc(const acr_nav::FDetailsrc &rhs) = delete;
private:
    // func:acr_nav.FDetailsrc..Ctor
    inline               FDetailsrc() __attribute__((nothrow));
    // func:acr_nav.FDetailsrc..Dtor
    inline               ~FDetailsrc() __attribute__((nothrow));
    friend acr_nav::FDetailsrc& detailsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FDetailsrc* detailsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 detailsrc_RemoveAll() __attribute__((nothrow));
    friend void                 detailsrc_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FField - Specify field of a struct
<a href="#acr_nav-ffield"></a>

#### acr_nav.FField Fields
<a href="#acr_nav-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FField.base|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|acr_nav.FField.p_ctype|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_nav.FField.p_arg|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_nav.FField.p_reftype|[acr_nav.FReftype](/txt/exe/acr_nav/internals.md#acr_nav-freftype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``acr_nav.FField.base`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FField { // acr_nav.FField
    acr_nav::FField*     ind_field_next;             // hash next
    u32                  ind_field_hashval;          // hash value
    algo::Smallstr100    field;                      // Primary key, as ctype.name
    algo::Smallstr100    arg;                        // Type of field
    algo::Smallstr50     reftype;                    //   "Val"  Type constructor
    algo::CppExpr        dflt;                       // Default value (c++ expression)
    algo::Comment        comment;                    //
    acr_nav::FCtype*     p_ctype;                    // reference to parent row
    acr_nav::FCtype*     p_arg;                      // reference to parent row
    acr_nav::FReftype*   p_reftype;                  // reference to parent row
    bool                 ctype_c_field_in_ary;       //   false  membership flag
    bool                 ctype_c_field_arg_in_ary;   //   false  membership flag
    // x-reference on acr_nav.FField.p_ctype prevents copy
    // x-reference on acr_nav.FField.p_arg prevents copy
    // x-reference on acr_nav.FField.p_reftype prevents copy
    // func:acr_nav.FField..AssignOp
    acr_nav::FField&     operator =(const acr_nav::FField &rhs) = delete;
    // x-reference on acr_nav.FField.p_ctype prevents copy
    // x-reference on acr_nav.FField.p_arg prevents copy
    // x-reference on acr_nav.FField.p_reftype prevents copy
    // func:acr_nav.FField..CopyCtor
    FField(const acr_nav::FField &rhs) = delete;
private:
    // func:acr_nav.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:acr_nav.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend acr_nav::FField&     field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FField*     field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveAll() __attribute__((nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FFiltertarget - 
<a href="#acr_nav-ffiltertarget"></a>

#### acr_nav.FFiltertarget Fields
<a href="#acr_nav-ffiltertarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FFiltertarget.base|[acr_navdb.Filtertarget](/txt/ssimdb/acr_navdb/filtertarget.md)|[Base](/txt/ssimdb/acr_navdb/filtertarget.md)|||

#### Struct FFiltertarget
<a href="#struct-ffiltertarget"></a>
*Note:* field ``acr_nav.FFiltertarget.base`` has reftype ``base`` so the fields of [acr_navdb.Filtertarget](/txt/ssimdb/acr_navdb/filtertarget.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FFiltertarget { // acr_nav.FFiltertarget
    acr_nav::FFiltertarget*   ind_filtertarget_next;      // hash next
    u32                       ind_filtertarget_hashval;   // hash value
    algo::Smallstr50          filtertarget;               //
    algo::Smallstr16          label;                      // Status bar indicator prefix
    algo::Smallstr50          next;                       // Next filtertarget in Tab cycle
    algo::Smallstr50          description;                // Hint shown in filter mode status bar
    bool                      match_ctype_name;           //   false  Match against ctype name
    bool                      match_field_name;           //   false  Match against field name
    bool                      match_comment;              //   false  Match against field comment
    bool                      match_arg;                  //   false  Match against field argument type
    bool                      match_reftype;              //   false  Match against field reftype
    bool                      has_field_criteria;         //   false  Target searches field-level properties (enables right-panel highlighting)
    algo::Comment             comment;                    //
    // func:acr_nav.FFiltertarget..AssignOp
    acr_nav::FFiltertarget& operator =(const acr_nav::FFiltertarget &rhs) = delete;
    // func:acr_nav.FFiltertarget..CopyCtor
    FFiltertarget(const acr_nav::FFiltertarget &rhs) = delete;
private:
    // func:acr_nav.FFiltertarget..Ctor
    inline               FFiltertarget() __attribute__((nothrow));
    // func:acr_nav.FFiltertarget..Dtor
    inline               ~FFiltertarget() __attribute__((nothrow));
    friend acr_nav::FFiltertarget& filtertarget_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FFiltertarget* filtertarget_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 filtertarget_RemoveAll() __attribute__((nothrow));
    friend void                 filtertarget_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FHelpgroup - Help panel group category
<a href="#acr_nav-fhelpgroup"></a>

#### acr_nav.FHelpgroup Fields
<a href="#acr_nav-fhelpgroup-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FHelpgroup.base|[acr_navdb.Helpgroup](/txt/ssimdb/acr_navdb/helpgroup.md)|[Base](/txt/ssimdb/acr_navdb/helpgroup.md)|||

#### Struct FHelpgroup
<a href="#struct-fhelpgroup"></a>
*Note:* field ``acr_nav.FHelpgroup.base`` has reftype ``base`` so the fields of [acr_navdb.Helpgroup](/txt/ssimdb/acr_navdb/helpgroup.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FHelpgroup { // acr_nav.FHelpgroup
    acr_nav::FHelpgroup*   ind_helpgroup_next;      // hash next
    u32                    ind_helpgroup_hashval;   // hash value
    algo::Smallstr50       helpgroup;               //
    i32                    sort_order;              //   0  Group ordering in help panel
    algo::Comment          comment;                 //
    // func:acr_nav.FHelpgroup..AssignOp
    inline acr_nav::FHelpgroup& operator =(const acr_nav::FHelpgroup &rhs) = delete;
    // func:acr_nav.FHelpgroup..CopyCtor
    inline               FHelpgroup(const acr_nav::FHelpgroup &rhs) = delete;
private:
    // func:acr_nav.FHelpgroup..Ctor
    inline               FHelpgroup() __attribute__((nothrow));
    // func:acr_nav.FHelpgroup..Dtor
    inline               ~FHelpgroup() __attribute__((nothrow));
    friend acr_nav::FHelpgroup& helpgroup_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FHelpgroup* helpgroup_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 helpgroup_RemoveAll() __attribute__((nothrow));
    friend void                 helpgroup_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FKeybind - Key-to-action mapping for acr_nav
<a href="#acr_nav-fkeybind"></a>

#### acr_nav.FKeybind Fields
<a href="#acr_nav-fkeybind-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FKeybind.base|[acr_navdb.Keybind](/txt/ssimdb/acr_navdb/keybind.md)|[Base](/txt/ssimdb/acr_navdb/keybind.md)|||
|acr_nav.FKeybind.p_navaction|[acr_nav.FNavaction](/txt/exe/acr_nav/internals.md#acr_nav-fnavaction)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FKeybind
<a href="#struct-fkeybind"></a>
*Note:* field ``acr_nav.FKeybind.base`` has reftype ``base`` so the fields of [acr_navdb.Keybind](/txt/ssimdb/acr_navdb/keybind.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FKeybind { // acr_nav.FKeybind
    acr_nav::FKeybind*     ind_keybind_next;      // hash next
    u32                    ind_keybind_hashval;   // hash value
    algo::Smallstr50       keybind;               //
    algo::Smallstr50       navaction;             // Navigation action
    algo::Comment          comment;               //
    acr_nav::FNavaction*   p_navaction;           // reference to parent row
    // x-reference on acr_nav.FKeybind.p_navaction prevents copy
    // func:acr_nav.FKeybind..AssignOp
    inline acr_nav::FKeybind& operator =(const acr_nav::FKeybind &rhs) = delete;
    // x-reference on acr_nav.FKeybind.p_navaction prevents copy
    // func:acr_nav.FKeybind..CopyCtor
    inline               FKeybind(const acr_nav::FKeybind &rhs) = delete;
private:
    // func:acr_nav.FKeybind..Ctor
    inline               FKeybind() __attribute__((nothrow));
    // func:acr_nav.FKeybind..Dtor
    inline               ~FKeybind() __attribute__((nothrow));
    friend acr_nav::FKeybind&   keybind_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FKeybind*   keybind_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 keybind_RemoveAll() __attribute__((nothrow));
    friend void                 keybind_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FNavaction - Controlled vocabulary of navigation actions
<a href="#acr_nav-fnavaction"></a>

#### acr_nav.FNavaction Fields
<a href="#acr_nav-fnavaction-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FNavaction.base|[acr_navdb.Navaction](/txt/ssimdb/acr_navdb/navaction.md)|[Base](/txt/ssimdb/acr_navdb/navaction.md)|||
|acr_nav.FNavaction.p_helpgroup|[acr_nav.FHelpgroup](/txt/exe/acr_nav/internals.md#acr_nav-fhelpgroup)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_nav.FNavaction.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||

#### Struct FNavaction
<a href="#struct-fnavaction"></a>
*Note:* field ``acr_nav.FNavaction.base`` has reftype ``base`` so the fields of [acr_navdb.Navaction](/txt/ssimdb/acr_navdb/navaction.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FNavaction { // acr_nav.FNavaction
    acr_nav::FNavaction*           ind_navaction_next;      // hash next
    u32                            ind_navaction_hashval;   // hash value
    algo::Smallstr50               navaction;               //
    algo::Smallstr50               helpgroup;               // Help group; empty=hidden from help
    i32                            sort_order;              //   0  Sort order within help group
    bool                           passive;                 //   false  Movement-only action; does not dismiss startup help
    bool                           need_no_overlay;         //   false  Hint hidden when viewmode.is_overlay is Y
    algo::Smallstr50               dismiss_viewmode;        // Viewmode this action dismisses; empty means any overlay
    algo::Smallstr50               target_viewmode;         // Viewmode to toggle (empty=not a toggle action)
    algo::Comment                  comment;                 //
    acr_nav::FHelpgroup*           p_helpgroup;             // optional pointer
    acr_nav::navaction_step_hook   step;                    //   NULL  Pointer to a function
    // reftype Hook of acr_nav.FNavaction.step prohibits copy
    // func:acr_nav.FNavaction..AssignOp
    acr_nav::FNavaction& operator =(const acr_nav::FNavaction &rhs) = delete;
    // reftype Hook of acr_nav.FNavaction.step prohibits copy
    // func:acr_nav.FNavaction..CopyCtor
    FNavaction(const acr_nav::FNavaction &rhs) = delete;
private:
    // func:acr_nav.FNavaction..Ctor
    inline               FNavaction() __attribute__((nothrow));
    // func:acr_nav.FNavaction..Dtor
    inline               ~FNavaction() __attribute__((nothrow));
    friend acr_nav::FNavaction& navaction_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FNavaction* navaction_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 navaction_RemoveAll() __attribute__((nothrow));
    friend void                 navaction_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FNavmode - UI mode for acr_nav
<a href="#acr_nav-fnavmode"></a>

#### acr_nav.FNavmode Fields
<a href="#acr_nav-fnavmode-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FNavmode.base|[acr_navdb.Navmode](/txt/ssimdb/acr_navdb/navmode.md)|[Base](/txt/ssimdb/acr_navdb/navmode.md)|||

#### Struct FNavmode
<a href="#struct-fnavmode"></a>
*Note:* field ``acr_nav.FNavmode.base`` has reftype ``base`` so the fields of [acr_navdb.Navmode](/txt/ssimdb/acr_navdb/navmode.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FNavmode { // acr_nav.FNavmode
    acr_nav::FNavmode*   ind_navmode_next;      // hash next
    u32                  ind_navmode_hashval;   // hash value
    algo::Smallstr50     navmode;               //
    algo::Smallstr200    status_hint;           //   ""  Status bar hint for this mode
    algo::Comment        comment;               //
    // func:acr_nav.FNavmode..AssignOp
    inline acr_nav::FNavmode& operator =(const acr_nav::FNavmode &rhs) = delete;
    // func:acr_nav.FNavmode..CopyCtor
    inline               FNavmode(const acr_nav::FNavmode &rhs) = delete;
private:
    // func:acr_nav.FNavmode..Ctor
    inline               FNavmode() __attribute__((nothrow));
    // func:acr_nav.FNavmode..Dtor
    inline               ~FNavmode() __attribute__((nothrow));
    friend acr_nav::FNavmode&   navmode_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FNavmode*   navmode_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 navmode_RemoveAll() __attribute__((nothrow));
    friend void                 navmode_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FNavstyle - Terminal visual style for acr_nav UI elements
<a href="#acr_nav-fnavstyle"></a>

#### acr_nav.FNavstyle Fields
<a href="#acr_nav-fnavstyle-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FNavstyle.base|[acr_navdb.Navstyle](/txt/ssimdb/acr_navdb/navstyle.md)|[Base](/txt/ssimdb/acr_navdb/navstyle.md)|||

#### Struct FNavstyle
<a href="#struct-fnavstyle"></a>
*Note:* field ``acr_nav.FNavstyle.base`` has reftype ``base`` so the fields of [acr_navdb.Navstyle](/txt/ssimdb/acr_navdb/navstyle.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FNavstyle { // acr_nav.FNavstyle
    acr_nav::FNavstyle*   ind_navstyle_next;      // hash next
    u32                   ind_navstyle_hashval;   // hash value
    algo::Smallstr50      navstyle;               //
    bool                  bold;                   //   false
    bool                  dim;                    //   false
    bool                  reverse;                //   false
    algo::TermColor       fg_color;               // Foreground color
    algo::Comment         comment;                //
    // func:acr_nav.FNavstyle..AssignOp
    inline acr_nav::FNavstyle& operator =(const acr_nav::FNavstyle &rhs) = delete;
    // func:acr_nav.FNavstyle..CopyCtor
    inline               FNavstyle(const acr_nav::FNavstyle &rhs) = delete;
private:
    // func:acr_nav.FNavstyle..Ctor
    inline               FNavstyle() __attribute__((nothrow));
    // func:acr_nav.FNavstyle..Dtor
    inline               ~FNavstyle() __attribute__((nothrow));
    friend acr_nav::FNavstyle&  navstyle_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FNavstyle*  navstyle_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 navstyle_RemoveAll() __attribute__((nothrow));
    friend void                 navstyle_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#acr_nav-fns"></a>

#### acr_nav.FNs Fields
<a href="#acr_nav-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|acr_nav.FNs.collapsed|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Namespace collapsed in tree view|
|acr_nav.FNs.c_ctype|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Ctypes in this namespace|
|acr_nav.FNs.n_match|i32|[Val](/txt/exe/amc/reftypes.md#val)||Matching ctypes under current filter|
|acr_nav.FNs.auto_expanded|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Set when filter-accept auto-expands a collapsed namespace|

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``acr_nav.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FNs { // acr_nav.FNs
    acr_nav::FNs*       ind_ns_next;      // hash next
    u32                 ind_ns_hashval;   // hash value
    algo::Smallstr16    ns;               // Namespace name (primary key)
    algo::Smallstr50    nstype;           // Namespace type
    algo::Smallstr50    license;          // Associated license
    algo::Comment       comment;          //
    bool                collapsed;        //   true  Namespace collapsed in tree view
    acr_nav::FCtype**   c_ctype_elems;    // array of pointers
    u32                 c_ctype_n;        // array of pointers
    u32                 c_ctype_max;      // capacity of allocated array
    i32                 n_match;          //   0  Matching ctypes under current filter
    bool                auto_expanded;    //   false  Set when filter-accept auto-expands a collapsed namespace
    // reftype Ptrary of acr_nav.FNs.c_ctype prohibits copy
    // func:acr_nav.FNs..AssignOp
    inline acr_nav::FNs& operator =(const acr_nav::FNs &rhs) = delete;
    // reftype Ptrary of acr_nav.FNs.c_ctype prohibits copy
    // func:acr_nav.FNs..CopyCtor
    inline               FNs(const acr_nav::FNs &rhs) = delete;
private:
    // func:acr_nav.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:acr_nav.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend acr_nav::FNs&        ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FNs*        ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FPanel - Panel definition for acr_nav TUI layout
<a href="#acr_nav-fpanel"></a>

#### acr_nav.FPanel Fields
<a href="#acr_nav-fpanel-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FPanel.base|[acr_navdb.Panel](/txt/ssimdb/acr_navdb/panel.md)|[Base](/txt/ssimdb/acr_navdb/panel.md)|||
|acr_nav.FPanel.scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)||Scroll position|
|acr_nav.FPanel.sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Selected row|

#### Struct FPanel
<a href="#struct-fpanel"></a>
*Note:* field ``acr_nav.FPanel.base`` has reftype ``base`` so the fields of [acr_navdb.Panel](/txt/ssimdb/acr_navdb/panel.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FPanel { // acr_nav.FPanel
    acr_nav::FPanel*   ind_panel_next;      // hash next
    u32                ind_panel_hashval;   // hash value
    algo::Smallstr50   panel;               //
    algo::cstring      title;               // Display title
    i32                position;            //   0  Left-to-right ordering
    i32                min_width;           //   0  Minimum column width (0 = derived)
    algo::Comment      comment;             //
    i32                scroll_offset;       //   0  Scroll position
    i32                sel_row;             //   0  Selected row
    // func:acr_nav.FPanel..AssignOp
    inline acr_nav::FPanel& operator =(const acr_nav::FPanel &rhs) = delete;
    // func:acr_nav.FPanel..CopyCtor
    inline               FPanel(const acr_nav::FPanel &rhs) = delete;
private:
    // func:acr_nav.FPanel..Ctor
    inline               FPanel() __attribute__((nothrow));
    // func:acr_nav.FPanel..Dtor
    inline               ~FPanel() __attribute__((nothrow));
    friend acr_nav::FPanel&     panel_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FPanel*     panel_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 panel_RemoveAll() __attribute__((nothrow));
    friend void                 panel_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FReftype - Field type constructor (e.g. reference type)
<a href="#acr_nav-freftype"></a>

#### acr_nav.FReftype Fields
<a href="#acr_nav-freftype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FReftype.base|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Base](/txt/ssimdb/dmmeta/reftype.md)|||
|acr_nav.FReftype.c_reftypestyle|[acr_nav.FReftypestyle](/txt/exe/acr_nav/internals.md#acr_nav-freftypestyle)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FReftype
<a href="#struct-freftype"></a>
*Note:* field ``acr_nav.FReftype.base`` has reftype ``base`` so the fields of [dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FReftype { // acr_nav.FReftype
    acr_nav::FReftype*        ind_reftype_next;      // hash next
    u32                       ind_reftype_hashval;   // hash value
    algo::Smallstr50          reftype;               //   "Val"
    bool                      isval;                 //   false  True if field makes values of target type
    bool                      cascins;               //   false  Field is cascade-insert
    bool                      usebasepool;           //   false  Fields with this type make use of dmmeta.basepool
    bool                      cancopy;               //   false  This type of field can be copied
    bool                      isxref;                //   false  This type of field is an x-ref
    bool                      del;                   //   false  Supports random deletion?
    bool                      up;                    //   false  This type of field is a reference
    bool                      isnew;                 //   false  If set, skip this relation in amc_vis
    bool                      hasalloc;              //   false  Generte Alloc/Delete functions for arg type
    bool                      inst;                  //   false  Field creates an instance of arg type (directly or indirectly)
    bool                      varlen;                //   false  This pool supports varlen allocations
    algo::Comment             comment;               //
    acr_nav::FReftypestyle*   c_reftypestyle;        // optional pointer
    // x-reference on acr_nav.FReftype.c_reftypestyle prevents copy
    // func:acr_nav.FReftype..AssignOp
    acr_nav::FReftype&   operator =(const acr_nav::FReftype &rhs) = delete;
    // x-reference on acr_nav.FReftype.c_reftypestyle prevents copy
    // func:acr_nav.FReftype..CopyCtor
    FReftype(const acr_nav::FReftype &rhs) = delete;
private:
    // func:acr_nav.FReftype..Ctor
    inline               FReftype() __attribute__((nothrow));
    // func:acr_nav.FReftype..Dtor
    inline               ~FReftype() __attribute__((nothrow));
    friend acr_nav::FReftype&   reftype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FReftype*   reftype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 reftype_RemoveAll() __attribute__((nothrow));
    friend void                 reftype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FReftypestyle - Mapping from reftype to navstyle for field coloring
<a href="#acr_nav-freftypestyle"></a>

#### acr_nav.FReftypestyle Fields
<a href="#acr_nav-freftypestyle-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FReftypestyle.base|[acr_navdb.Reftypestyle](/txt/ssimdb/acr_navdb/reftypestyle.md)|[Base](/txt/ssimdb/acr_navdb/reftypestyle.md)|||
|acr_nav.FReftypestyle.p_navstyle|[acr_nav.FNavstyle](/txt/exe/acr_nav/internals.md#acr_nav-fnavstyle)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FReftypestyle
<a href="#struct-freftypestyle"></a>
*Note:* field ``acr_nav.FReftypestyle.base`` has reftype ``base`` so the fields of [acr_navdb.Reftypestyle](/txt/ssimdb/acr_navdb/reftypestyle.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FReftypestyle { // acr_nav.FReftypestyle
    acr_nav::FReftypestyle*   ind_reftypestyle_next;      // hash next
    u32                       ind_reftypestyle_hashval;   // hash value
    algo::Smallstr50          reftypestyle;               //
    algo::Comment             comment;                    //
    acr_nav::FNavstyle*       p_navstyle;                 // reference to parent row
    // x-reference on acr_nav.FReftypestyle.p_navstyle prevents copy
    // func:acr_nav.FReftypestyle..AssignOp
    inline acr_nav::FReftypestyle& operator =(const acr_nav::FReftypestyle &rhs) = delete;
    // x-reference on acr_nav.FReftypestyle.p_navstyle prevents copy
    // func:acr_nav.FReftypestyle..CopyCtor
    inline               FReftypestyle(const acr_nav::FReftypestyle &rhs) = delete;
private:
    // func:acr_nav.FReftypestyle..Ctor
    inline               FReftypestyle() __attribute__((nothrow));
    // func:acr_nav.FReftypestyle..Dtor
    inline               ~FReftypestyle() __attribute__((nothrow));
    friend acr_nav::FReftypestyle& reftypestyle_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FReftypestyle* reftypestyle_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 reftypestyle_RemoveAll() __attribute__((nothrow));
    friend void                 reftypestyle_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FSsimfile - File with ssim tuples
<a href="#acr_nav-fssimfile"></a>

#### acr_nav.FSsimfile Fields
<a href="#acr_nav-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|acr_nav.FSsimfile.p_ctype|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_nav.FSsimfile.n_record|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records (counted at startup)|

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``acr_nav.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FSsimfile { // acr_nav.FSsimfile
    acr_nav::FSsimfile*   ind_ssimfile_next;      // hash next
    u32                   ind_ssimfile_hashval;   // hash value
    algo::Smallstr50      ssimfile;               //
    algo::Smallstr100     ctype;                  //
    acr_nav::FCtype*      p_ctype;                // reference to parent row
    i32                   n_record;               //   0  Number of records (counted at startup)
    // x-reference on acr_nav.FSsimfile.p_ctype prevents copy
    // func:acr_nav.FSsimfile..AssignOp
    inline acr_nav::FSsimfile& operator =(const acr_nav::FSsimfile &rhs) = delete;
    // x-reference on acr_nav.FSsimfile.p_ctype prevents copy
    // func:acr_nav.FSsimfile..CopyCtor
    inline               FSsimfile(const acr_nav::FSsimfile &rhs) = delete;
private:
    // func:acr_nav.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:acr_nav.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend acr_nav::FSsimfile&  ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FSsimfile*  ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveAll() __attribute__((nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.FViewmode - Right-panel view mode for acr_nav
<a href="#acr_nav-fviewmode"></a>

#### acr_nav.FViewmode Fields
<a href="#acr_nav-fviewmode-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.FViewmode.base|[acr_navdb.Viewmode](/txt/ssimdb/acr_navdb/viewmode.md)|[Base](/txt/ssimdb/acr_navdb/viewmode.md)|||
|acr_nav.FViewmode.header|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Column header line|
|acr_nav.FViewmode.cspan|[acr_nav.LineColorSpan](/txt/exe/acr_nav/internals.md#acr_nav-linecolorspan)|[Tary](/txt/exe/amc/reftypes.md#tary)||Color spans for line-mode highlighting|
|acr_nav.FViewmode.ensure_content|[acr_nav.FCtype](/txt/exe/acr_nav/internals.md#acr_nav-fctype)|[Hook](/txt/exe/amc/reftypes.md#hook)||Called with selected ctype to ensure view content is loaded|
|acr_nav.FViewmode.nav_col|[acr_nav.PreviewNavCol](/txt/exe/acr_nav/internals.md#acr_nav-previewnavcol)|[Tary](/txt/exe/amc/reftypes.md#tary)||Navigable columns in current preview|
|acr_nav.FViewmode.pkey_wid|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Column 0 (pkey) width for target row matching|
|acr_nav.FViewmode.preview_h_scroll|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Horizontal scroll offset (display columns)|
|acr_nav.FViewmode.total_content_wid|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Total formatted content width (display columns)|
|acr_nav.FViewmode.cached_key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Cache key (ctype or ns key) for content invalidation|
|acr_nav.FViewmode.content_row|[acr_nav.ContentRow](/txt/exe/acr_nav/internals.md#acr_nav-contentrow)|[Tary](/txt/exe/amc/reftypes.md#tary)||Content rows for has_fields:N viewmodes|

#### Struct FViewmode
<a href="#struct-fviewmode"></a>
*Note:* field ``acr_nav.FViewmode.base`` has reftype ``base`` so the fields of [acr_navdb.Viewmode](/txt/ssimdb/acr_navdb/viewmode.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct FViewmode { // acr_nav.FViewmode
    acr_nav::FViewmode*                     ind_viewmode_next;      // hash next
    u32                                     ind_viewmode_hashval;   // hash value
    algo::Smallstr50                        viewmode;               //
    algo::Smallstr50                        title;                  // Right panel title for this viewmode
    algo::Smallstr50                        next;                   // Next viewmode in Tab cycle
    algo::Smallstr50                        empty_msg;              // Message shown when right panel has no items
    bool                                    has_fields;             //   false  Y: renders field records; N: renders preformatted lines
    bool                                    is_overlay;             //   false  Y: overlay viewmode (help, detail); suppresses need_no_overlay hints
    bool                                    need_ssimfile;          //   false  Viewmode requires ssimfile-backed ctype
    bool                                    is_reverse;             //   false  Y=reverse xrefs, N=forward fields
    algo::Smallstr200                       status_hint;            //   ""  Status bar hint (right panel for views, full hint for overlays)
    bool                                    scope_ns;               //   false  Y: viewmode is scoped to namespace (auto-activate/deactivate on ns headers)
    algo::Comment                           comment;                //
    algo::cstring                           header;                 // Column header line
    acr_nav::LineColorSpan*                 cspan_elems;            // pointer to elements
    u32                                     cspan_n;                // number of elements in array
    u32                                     cspan_max;              // max. capacity of array before realloc
    acr_nav::viewmode_ensure_content_hook   ensure_content;         //   NULL  Pointer to a function
    acr_nav::PreviewNavCol*                 nav_col_elems;          // pointer to elements
    u32                                     nav_col_n;              // number of elements in array
    u32                                     nav_col_max;            // max. capacity of array before realloc
    i32                                     pkey_wid;               //   0  Column 0 (pkey) width for target row matching
    i32                                     preview_h_scroll;       //   0  Horizontal scroll offset (display columns)
    i32                                     total_content_wid;      //   0  Total formatted content width (display columns)
    algo::cstring                           cached_key;             //   ""  Cache key (ctype or ns key) for content invalidation
    acr_nav::ContentRow*                    content_row_elems;      // pointer to elements
    u32                                     content_row_n;          // number of elements in array
    u32                                     content_row_max;        // max. capacity of array before realloc
    // reftype Hook of acr_nav.FViewmode.ensure_content prohibits copy
    // func:acr_nav.FViewmode..AssignOp
    acr_nav::FViewmode&  operator =(const acr_nav::FViewmode &rhs) = delete;
    // reftype Hook of acr_nav.FViewmode.ensure_content prohibits copy
    // func:acr_nav.FViewmode..CopyCtor
    FViewmode(const acr_nav::FViewmode &rhs) = delete;
private:
    // func:acr_nav.FViewmode..Ctor
    inline               FViewmode() __attribute__((nothrow));
    // func:acr_nav.FViewmode..Dtor
    inline               ~FViewmode() __attribute__((nothrow));
    friend acr_nav::FViewmode&  viewmode_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_nav::FViewmode*  viewmode_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 viewmode_RemoveAll() __attribute__((nothrow));
    friend void                 viewmode_RemoveLast() __attribute__((nothrow));
};
```

#### acr_nav.GoBack - Headless command: pop navigation stack
<a href="#acr_nav-goback"></a>

#### acr_nav.GoBack Fields
<a href="#acr_nav-goback-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.GoBack.go_back|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct GoBack
<a href="#struct-goback"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct GoBack { // acr_nav.GoBack: Headless command: pop navigation stack
    algo::Smallstr50   go_back;   //
    // func:acr_nav.GoBack..Ctor
    inline               GoBack() __attribute__((nothrow));
};
```

#### acr_nav.InputError - Headless error output for unrecognized input
<a href="#acr_nav-inputerror"></a>

#### acr_nav.InputError Fields
<a href="#acr_nav-inputerror-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.InputError.lineno|i32|[Val](/txt/exe/amc/reftypes.md#val)||Input line number (1-based)|
|acr_nav.InputError.msg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Error message|

#### Struct InputError
<a href="#struct-inputerror"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct InputError { // acr_nav.InputError: Headless error output for unrecognized input
    i32             lineno;   //   0  Input line number (1-based)
    algo::cstring   msg;      // Error message
    // func:acr_nav.InputError..Ctor
    inline               InputError() __attribute__((nothrow));
};
```

#### acr_nav.LeftItem - One display row in the left panel
<a href="#acr_nav-leftitem"></a>

#### acr_nav.LeftItem Fields
<a href="#acr_nav-leftitem-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.LeftItem.ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Ctype key (empty for namespace header)|
|acr_nav.LeftItem.ns|[algo.Smallstr16](/txt/protocol/algo/README.md#algo-smallstr16)|[Val](/txt/exe/amc/reftypes.md#val)||Namespace key (set for namespace headers)|

#### Struct LeftItem
<a href="#struct-leftitem"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct LeftItem { // acr_nav.LeftItem: One display row in the left panel
    algo::Smallstr100   ctype;   // Ctype key (empty for namespace header)
    algo::Smallstr16    ns;      // Namespace key (set for namespace headers)
    // func:acr_nav.LeftItem..Ctor
    inline               LeftItem() __attribute__((nothrow));
};
```

#### acr_nav.LineColorSpan - 
<a href="#acr_nav-linecolorspan"></a>

#### acr_nav.LineColorSpan Fields
<a href="#acr_nav-linecolorspan-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.LineColorSpan.line_idx|i32|[Val](/txt/exe/amc/reftypes.md#val)||Line index in viewmode line Tary|
|acr_nav.LineColorSpan.col_start|i32|[Val](/txt/exe/amc/reftypes.md#val)||Start column, 0-based relative to stored line text|
|acr_nav.LineColorSpan.col_end|i32|[Val](/txt/exe/amc/reftypes.md#val)||End column (exclusive)|
|acr_nav.LineColorSpan.p_navstyle|[acr_nav.FNavstyle](/txt/exe/acr_nav/internals.md#acr_nav-fnavstyle)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Style to apply (set manually by loader)|

#### Struct LineColorSpan
<a href="#struct-linecolorspan"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct LineColorSpan { // acr_nav.LineColorSpan
    i32                   line_idx;     //   0  Line index in viewmode line Tary
    i32                   col_start;    //   0  Start column, 0-based relative to stored line text
    i32                   col_end;      //   0  End column (exclusive)
    acr_nav::FNavstyle*   p_navstyle;   // reference to parent row
    // func:acr_nav.LineColorSpan..Ctor
    inline               LineColorSpan() __attribute__((nothrow));
};
```

#### acr_nav.Naventry - Navigation stack entry. Uses raw strings not Pkeys: value type in Tary, stores snapshots not live references
<a href="#acr_nav-naventry"></a>

#### acr_nav.Naventry Fields
<a href="#acr_nav-naventry-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Naventry.filter|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Filter text at time of push|
|acr_nav.Naventry.navmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Navigation mode at time of push|
|acr_nav.Naventry.scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.Naventry.sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.Naventry.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Viewmode at time of push|
|acr_nav.Naventry.ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Ctype being viewed at time of push|
|acr_nav.Naventry.filtertarget|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Filter target at time of push|
|acr_nav.Naventry.right_sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Right panel selection row|
|acr_nav.Naventry.right_scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Right panel scroll offset|
|acr_nav.Naventry.focus_panel|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Panel name that had focus at time of push|
|acr_nav.Naventry.sel_nav_col|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Selected navigable column at time of push|

#### Struct Naventry
<a href="#struct-naventry"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Naventry { // acr_nav.Naventry: Navigation stack entry. Uses raw strings not Pkeys: value type in Tary, stores snapshots not live references
    algo::cstring       filter;                // Filter text at time of push
    algo::Smallstr50    navmode;               // Navigation mode at time of push
    i32                 scroll_offset;         //   0
    i32                 sel_row;               //   0
    algo::Smallstr50    viewmode;              // Viewmode at time of push
    algo::Smallstr100   ctype;                 // Ctype being viewed at time of push
    algo::Smallstr50    filtertarget;          // Filter target at time of push
    i32                 right_sel_row;         //   0  Right panel selection row
    i32                 right_scroll_offset;   //   0  Right panel scroll offset
    algo::Smallstr50    focus_panel;           // Panel name that had focus at time of push
    i32                 sel_nav_col;           //   0  Selected navigable column at time of push
    // func:acr_nav.Naventry..Ctor
    inline               Naventry() __attribute__((nothrow));
};
```

#### acr_nav.Navigate - Headless command: navigate to a ctype by name
<a href="#acr_nav-navigate"></a>

#### acr_nav.Navigate Fields
<a href="#acr_nav-navigate-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Navigate.navigate|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.Navigate.ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Target ctype|
|acr_nav.Navigate.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Optional viewmode|

#### Struct Navigate
<a href="#struct-navigate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Navigate { // acr_nav.Navigate: Headless command: navigate to a ctype by name
    algo::Smallstr50    navigate;   //
    algo::Smallstr100   ctype;      // Target ctype
    algo::Smallstr50    viewmode;   // Optional viewmode
    // func:acr_nav.Navigate..Ctor
    inline               Navigate() __attribute__((nothrow));
};
```

#### acr_nav.OverlayEntry - Overlay stack entry: viewmode name + saved right-panel state
<a href="#acr_nav-overlayentry"></a>

#### acr_nav.OverlayEntry Fields
<a href="#acr_nav-overlayentry-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.OverlayEntry.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Viewmode name to restore|
|acr_nav.OverlayEntry.saved_sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Right-panel sel_row at time of push|
|acr_nav.OverlayEntry.saved_scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Right-panel scroll_offset at time of push|

#### Struct OverlayEntry
<a href="#struct-overlayentry"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct OverlayEntry { // acr_nav.OverlayEntry: Overlay stack entry: viewmode name + saved right-panel state
    algo::Smallstr50   viewmode;              // Viewmode name to restore
    i32                saved_sel_row;         //   0  Right-panel sel_row at time of push
    i32                saved_scroll_offset;   //   0  Right-panel scroll_offset at time of push
    // func:acr_nav.OverlayEntry..Ctor
    inline               OverlayEntry() __attribute__((nothrow));
};
```

#### acr_nav.PanelState - Headless panel state output
<a href="#acr_nav-panelstate"></a>

#### acr_nav.PanelState Fields
<a href="#acr_nav-panelstate-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.PanelState.panel|[acr_navdb.Panel](/txt/ssimdb/acr_navdb/panel.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Panel name|
|acr_nav.PanelState.sel_row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Selected row index|
|acr_nav.PanelState.scroll_offset|i32|[Val](/txt/exe/amc/reftypes.md#val)||Scroll position|
|acr_nav.PanelState.n_items|i32|[Val](/txt/exe/amc/reftypes.md#val)||Total item count|
|acr_nav.PanelState.sel_value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Display value of selected item|

#### Struct PanelState
<a href="#struct-panelstate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct PanelState { // acr_nav.PanelState: Headless panel state output
    algo::Smallstr50   panel;           // Panel name
    i32                sel_row;         //   0  Selected row index
    i32                scroll_offset;   //   0  Scroll position
    i32                n_items;         //   0  Total item count
    algo::cstring      sel_value;       // Display value of selected item
    // func:acr_nav.PanelState..Ctor
    inline               PanelState() __attribute__((nothrow));
};
```

#### acr_nav.PreviewNavCol - Navigable column metadata for preview follow-ref
<a href="#acr_nav-previewnavcol"></a>

#### acr_nav.PreviewNavCol Fields
<a href="#acr_nav-previewnavcol-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.PreviewNavCol.col_start|i32|[Val](/txt/exe/amc/reftypes.md#val)||Character start position in formatted line|
|acr_nav.PreviewNavCol.col_wid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Column width (for cell value extraction)|
|acr_nav.PreviewNavCol.name_len|i32|[Val](/txt/exe/amc/reftypes.md#val)||Length of column name in header|
|acr_nav.PreviewNavCol.col_name|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Column/attribute name|
|acr_nav.PreviewNavCol.target_ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Target ctype key for navigation|

#### Struct PreviewNavCol
<a href="#struct-previewnavcol"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct PreviewNavCol { // acr_nav.PreviewNavCol: Navigable column metadata for preview follow-ref
    i32                 col_start;      //   0  Character start position in formatted line
    i32                 col_wid;        //   0  Column width (for cell value extraction)
    i32                 name_len;       //   0  Length of column name in header
    algo::Smallstr50    col_name;       // Column/attribute name
    algo::Smallstr100   target_ctype;   // Target ctype key for navigation
    // func:acr_nav.PreviewNavCol..Ctor
    inline               PreviewNavCol() __attribute__((nothrow));
};
```

#### acr_nav.Screen - Headless screen state output
<a href="#acr_nav-screen"></a>

#### acr_nav.Screen Fields
<a href="#acr_nav-screen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Screen.mode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Current UI mode (browse/filter)|
|acr_nav.Screen.focus|[acr_navdb.Panel](/txt/ssimdb/acr_navdb/panel.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Currently focused panel|
|acr_nav.Screen.filter|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Current filter text|
|acr_nav.Screen.navstack_depth|i32|[Val](/txt/exe/amc/reftypes.md#val)||Navigation stack depth|
|acr_nav.Screen.n_sel_ctype|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of ctypes matching filter|
|acr_nav.Screen.n_ctype|i32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of ctypes|
|acr_nav.Screen.n_field|i32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of fields|
|acr_nav.Screen.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Current right-panel viewmode|
|acr_nav.Screen.breadcrumb|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Navigation breadcrumb trail (display string)|
|acr_nav.Screen.filtertarget|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Current filter target (ctype/field)|
|acr_nav.Screen.hints|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Status bar hint string|
|acr_nav.Screen.sel_nav_col|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Selected navigable column index in preview mode|

#### Struct Screen
<a href="#struct-screen"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Screen { // acr_nav.Screen: Headless screen state output
    algo::Smallstr50   mode;             // Current UI mode (browse/filter)
    algo::Smallstr50   focus;            // Currently focused panel
    algo::cstring      filter;           // Current filter text
    i32                navstack_depth;   //   0  Navigation stack depth
    i32                n_sel_ctype;      //   0  Number of ctypes matching filter
    i32                n_ctype;          //   0  Total number of ctypes
    i32                n_field;          //   0  Total number of fields
    algo::Smallstr50   viewmode;         // Current right-panel viewmode
    algo::cstring      breadcrumb;       // Navigation breadcrumb trail (display string)
    algo::Smallstr50   filtertarget;     // Current filter target (ctype/field)
    algo::cstring      hints;            // Status bar hint string
    i32                sel_nav_col;      //   0  Selected navigable column index in preview mode
    // func:acr_nav.Screen..Ctor
    inline               Screen() __attribute__((nothrow));
};
```

#### acr_nav.Screenshot - Headless command: emit screen state
<a href="#acr_nav-screenshot"></a>

#### acr_nav.Screenshot Fields
<a href="#acr_nav-screenshot-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Screenshot.screenshot|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Screenshot
<a href="#struct-screenshot"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Screenshot { // acr_nav.Screenshot: Headless command: emit screen state
    algo::Smallstr50   screenshot;   //
    // func:acr_nav.Screenshot..Ctor
    inline               Screenshot() __attribute__((nothrow));
};
```

#### acr_nav.SendKey - Headless command: send keystroke
<a href="#acr_nav-sendkey"></a>

#### acr_nav.SendKey Fields
<a href="#acr_nav-sendkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.SendKey.key|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Key name to send|

#### Struct SendKey
<a href="#struct-sendkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct SendKey { // acr_nav.SendKey: Headless command: send keystroke
    algo::Smallstr50   key;   // Key name to send
    // func:acr_nav.SendKey..Ctor
    inline               SendKey() __attribute__((nothrow));
};
```

#### acr_nav.SetFilter - Headless command: apply filter atomically
<a href="#acr_nav-setfilter"></a>

#### acr_nav.SetFilter Fields
<a href="#acr_nav-setfilter-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.SetFilter.set_filter|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.SetFilter.filter|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Filter text|
|acr_nav.SetFilter.target|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|"ctype"|Filter target|

#### Struct SetFilter
<a href="#struct-setfilter"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct SetFilter { // acr_nav.SetFilter: Headless command: apply filter atomically
    algo::Smallstr50   set_filter;   //
    algo::cstring      filter;       // Filter text
    algo::Smallstr50   target;       //   "ctype"  Filter target
    // func:acr_nav.SetFilter..Ctor
    inline               SetFilter() __attribute__((nothrow));
};
```

#### acr_nav.SetTermSize - Headless command: set terminal dimensions
<a href="#acr_nav-settermsize"></a>

#### acr_nav.SetTermSize Fields
<a href="#acr_nav-settermsize-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.SetTermSize.term_hei|i32|[Val](/txt/exe/amc/reftypes.md#val)||Terminal height in rows|
|acr_nav.SetTermSize.term_wid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Terminal width in columns|

#### Struct SetTermSize
<a href="#struct-settermsize"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct SetTermSize { // acr_nav.SetTermSize: Headless command: set terminal dimensions
    i32   term_hei;   //   0  Terminal height in rows
    i32   term_wid;   //   0  Terminal width in columns
    // func:acr_nav.SetTermSize..Ctor
    inline               SetTermSize() __attribute__((nothrow));
};
```

#### acr_nav.SetView - Headless command: switch viewmode directly
<a href="#acr_nav-setview"></a>

#### acr_nav.SetView Fields
<a href="#acr_nav-setview-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.SetView.set_view|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_nav.SetView.viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Target viewmode|

#### Struct SetView
<a href="#struct-setview"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct SetView { // acr_nav.SetView: Headless command: switch viewmode directly
    algo::Smallstr50   set_view;   //
    algo::Smallstr50   viewmode;   // Target viewmode
    // func:acr_nav.SetView..Ctor
    inline               SetView() __attribute__((nothrow));
};
```

#### acr_nav.Summary - Headless command: emit lightweight state summary
<a href="#acr_nav-summary"></a>

#### acr_nav.Summary Fields
<a href="#acr_nav-summary-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.Summary.summary|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Summary
<a href="#struct-summary"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct Summary { // acr_nav.Summary: Headless command: emit lightweight state summary
    algo::Smallstr50   summary;   //
    // func:acr_nav.Summary..Ctor
    inline               Summary() __attribute__((nothrow));
};
```

#### acr_nav.VisibleField - Headless field output
<a href="#acr_nav-visiblefield"></a>

#### acr_nav.VisibleField Fields
<a href="#acr_nav-visiblefield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.VisibleField.row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Row index in field list|
|acr_nav.VisibleField.field|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Fully qualified field name|
|acr_nav.VisibleField.arg|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Argument ctype|
|acr_nav.VisibleField.reftype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Reference type|
|acr_nav.VisibleField.style|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Visual style name|
|acr_nav.VisibleField.navigable|bool|[Val](/txt/exe/amc/reftypes.md#val)||Whether follow_ref would navigate|
|acr_nav.VisibleField.match|bool|[Val](/txt/exe/amc/reftypes.md#val)||Field matches active filter (filtertarget:field)|

#### Struct VisibleField
<a href="#struct-visiblefield"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct VisibleField { // acr_nav.VisibleField: Headless field output
    i32                 row;         //   0  Row index in field list
    algo::Smallstr100   field;       // Fully qualified field name
    algo::Smallstr100   arg;         // Argument ctype
    algo::Smallstr50    reftype;     // Reference type
    algo::Smallstr50    style;       // Visual style name
    bool                navigable;   //   false  Whether follow_ref would navigate
    bool                match;       //   false  Field matches active filter (filtertarget:field)
    // func:acr_nav.VisibleField..Ctor
    inline               VisibleField() __attribute__((nothrow));
};
```

#### acr_nav.VisibleLeftItem - Headless output: one left panel row
<a href="#acr_nav-visibleleftitem"></a>

#### acr_nav.VisibleLeftItem Fields
<a href="#acr_nav-visibleleftitem-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.VisibleLeftItem.row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Row index in left panel (0-based)|
|acr_nav.VisibleLeftItem.value|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Namespace key or ctype key|
|acr_nav.VisibleLeftItem.kind|[algo.Smallstr16](/txt/protocol/algo/README.md#algo-smallstr16)|[Val](/txt/exe/amc/reftypes.md#val)||Row kind: ns or ctype|
|acr_nav.VisibleLeftItem.collapsed|bool|[Val](/txt/exe/amc/reftypes.md#val)||Whether namespace is collapsed (N for ctype rows)|
|acr_nav.VisibleLeftItem.n_match|i32|[Val](/txt/exe/amc/reftypes.md#val)||Matching ctypes in namespace (0 for ctype rows)|
|acr_nav.VisibleLeftItem.n_record|i32|[Val](/txt/exe/amc/reftypes.md#val)||Record count for ctypes with ssimfiles (0 otherwise)|

#### Struct VisibleLeftItem
<a href="#struct-visibleleftitem"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct VisibleLeftItem { // acr_nav.VisibleLeftItem: Headless output: one left panel row
    i32                 row;         //   0  Row index in left panel (0-based)
    algo::Smallstr100   value;       // Namespace key or ctype key
    algo::Smallstr16    kind;        // Row kind: ns or ctype
    bool                collapsed;   //   false  Whether namespace is collapsed (N for ctype rows)
    i32                 n_match;     //   0  Matching ctypes in namespace (0 for ctype rows)
    i32                 n_record;    //   0  Record count for ctypes with ssimfiles (0 otherwise)
    // func:acr_nav.VisibleLeftItem..Ctor
    inline               VisibleLeftItem() __attribute__((nothrow));
};
```

#### acr_nav.VisibleLine - Headless output: text line for non-field viewmodes
<a href="#acr_nav-visibleline"></a>

#### acr_nav.VisibleLine Fields
<a href="#acr_nav-visibleline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_nav.VisibleLine.row|i32|[Val](/txt/exe/amc/reftypes.md#val)||Row index|
|acr_nav.VisibleLine.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Line text|

#### Struct VisibleLine
<a href="#struct-visibleline"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_nav_gen.h](/include/gen/acr_nav_gen.h)
```
struct VisibleLine { // acr_nav.VisibleLine: Headless output: text line for non-field viewmodes
    i32             row;     //   0  Row index
    algo::cstring   value;   // Line text
    // func:acr_nav.VisibleLine..Ctor
    inline               VisibleLine() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

### Tests
<a href="#tests"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Tests -->
The following component tests are defined for `acr_nav`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[acr_nav.AllFilter](/test/atf_comp/acr_nav.AllFilter)|All filter: Tab to all target, type filter, verify matches across all properties|
|[acr_nav.ArgFilter](/test/atf_comp/acr_nav.ArgFilter)|Arg filter: Tab to arg target, type filter, verify match|
|[acr_nav.ArgFilterNavstack](/test/atf_comp/acr_nav.ArgFilterNavstack)|Arg filter navstack: navigate with arg filter, verify preserved on go-back|
|[acr_nav.Breadcrumb](/test/atf_comp/acr_nav.Breadcrumb)|Breadcrumb trail at multiple depths|
|[acr_nav.CachedKeyInvalidation](/test/atf_comp/acr_nav.CachedKeyInvalidation)|Content cache clears when cursor moves to namespace header|
|[acr_nav.Codegen](/test/atf_comp/acr_nav.Codegen)|Generated code preview|
|[acr_nav.Collapse](/test/atf_comp/acr_nav.Collapse)|Namespace collapse and expand|
|[acr_nav.Detail](/test/atf_comp/acr_nav.Detail)|Field detail drilldown and overlay nesting|
|[acr_nav.FieldFilter](/test/atf_comp/acr_nav.FieldFilter)|Field search: Tab to field target, type filter, verify match count|
|[acr_nav.FieldFilterCancel](/test/atf_comp/acr_nav.FieldFilterCancel)|Field search cancel resets filtertarget to ctype|
|[acr_nav.FieldFilterNavstack](/test/atf_comp/acr_nav.FieldFilterNavstack)|Field search accept preserves filtertarget, Escape clears it|
|[acr_nav.FieldFilterScreen](/test/atf_comp/acr_nav.FieldFilterScreen)|Field search filtertarget appears in headless Screen output|
|[acr_nav.Filter](/test/atf_comp/acr_nav.Filter)|Filter mode|
|[acr_nav.FilterAccept](/test/atf_comp/acr_nav.FilterAccept)|Filter accept expands namespaces and navigates to first match|
|[acr_nav.FilterCycleComplete](/test/atf_comp/acr_nav.FilterCycleComplete)|Full Tab cycle (ctype->field->arg->reftype->all->ctype); tmsg 099990.in SetTermSize 100000x120 for full visible left panel|
|[acr_nav.FilterDescription](/test/atf_comp/acr_nav.FilterDescription)|Filter description hint appears in status bar during filter mode|
|[acr_nav.FilterEdge](/test/atf_comp/acr_nav.FilterEdge)|Filter to zero matches and empty list navigation|
|[acr_nav.Follow](/test/atf_comp/acr_nav.Follow)|Follow reference|
|[acr_nav.GoBackBasic](/test/atf_comp/acr_nav.GoBackBasic)|Navigate twice then GoBack, verify navstack depth|
|[acr_nav.GoBackEmpty](/test/atf_comp/acr_nav.GoBackEmpty)|GoBack with empty navstack, verify error Ack|
|[acr_nav.GraphFollow](/test/atf_comp/acr_nav.GraphFollow)|Graph follow-ref via ContentRow nav_target + GoBack|
|[acr_nav.Help](/test/atf_comp/acr_nav.Help)|Help overlay toggle|
|[acr_nav.Init](/test/atf_comp/acr_nav.Init)|Auto-screenshot on empty stdin|
|[acr_nav.LeftItems](/test/atf_comp/acr_nav.LeftItems)|Left panel collapse/expand/filter; tmsg 099990.in SetTermSize 100000x120 for full visible left panel|
|[acr_nav.MalformedInput](/test/atf_comp/acr_nav.MalformedInput)|Error output for unrecognized input lines|
|[acr_nav.Nav](/test/atf_comp/acr_nav.Nav)|Navigation|
|[acr_nav.NavigateBasic](/test/atf_comp/acr_nav.NavigateBasic)|Navigate to dmmeta.Ctype, verify Ack + auto-screenshot on EOF|
|[acr_nav.NavigateNotFound](/test/atf_comp/acr_nav.NavigateNotFound)|Navigate to invalid ctype, verify error Ack|
|[acr_nav.NavigateWithView](/test/atf_comp/acr_nav.NavigateWithView)|Navigate with viewmode parameter|
|[acr_nav.NsDepDetail](/test/atf_comp/acr_nav.NsDepDetail)|nsdep_detail content loading and Tab cycle (nsdep->nsdep_detail->nsdep)|
|[acr_nav.NsDepDetailFollow](/test/atf_comp/acr_nav.NsDepDetailFollow)|Follow-ref from nsdep_detail field row + GoBack restores state|
|[acr_nav.NsDepDetailHeaderEnter](/test/atf_comp/acr_nav.NsDepDetailHeaderEnter)|Enter on nsdep_detail section header is no-op|
|[acr_nav.Preview](/test/atf_comp/acr_nav.Preview)|Preview mode with table formatting|
|[acr_nav.PreviewCharset](/test/atf_comp/acr_nav.PreviewCharset)|Preview sanitizes control characters in ssimfile values|
|[acr_nav.PreviewEmptyCell](/test/atf_comp/acr_nav.PreviewEmptyCell)|Enter on FK column with empty cell value is no-op|
|[acr_nav.PreviewHScroll](/test/atf_comp/acr_nav.PreviewHScroll)|Preview horizontal scroll cycles all columns|
|[acr_nav.PreviewNavCol](/test/atf_comp/acr_nav.PreviewNavCol)|Preview mode emits PreviewNavCol column metadata|
|[acr_nav.PreviewNavCycle](/test/atf_comp/acr_nav.PreviewNavCycle)|Preview nav: Right arrow cycles navigable columns|
|[acr_nav.PreviewNavFollow](/test/atf_comp/acr_nav.PreviewNavFollow)|Preview nav: Enter follows reference to target ssimfile|
|[acr_nav.PreviewNavNone](/test/atf_comp/acr_nav.PreviewNavNone)|Preview nav: no cycling on tables without navigable columns|
|[acr_nav.RecordCount](/test/atf_comp/acr_nav.RecordCount)|Record count display for ctypes with ssimfiles|
|[acr_nav.ReftypeFilter](/test/atf_comp/acr_nav.ReftypeFilter)|Reftype filter: Tab to reftype target, type filter, verify match|
|[acr_nav.SemanticWorkflow](/test/atf_comp/acr_nav.SemanticWorkflow)|Full v2 workflow: Navigate, SetView, SetFilter, GoBack|
|[acr_nav.SetFilterBasic](/test/atf_comp/acr_nav.SetFilterBasic)|Apply filter via SetFilter command|
|[acr_nav.SetTermSizeShrink](/test/atf_comp/acr_nav.SetTermSizeShrink)|AdjustScroll after SetTermSize shrink|
|[acr_nav.SetViewBasic](/test/atf_comp/acr_nav.SetViewBasic)|Switch viewmode via SetView command|
|[acr_nav.SetViewReject](/test/atf_comp/acr_nav.SetViewReject)|Reject nsdep and detail viewmodes via SetView|
|[acr_nav.SetViewRejectNsDepDetail](/test/atf_comp/acr_nav.SetViewRejectNsDepDetail)|SetView and Navigate reject nsdep_detail (scope_ns property)|
|[acr_nav.SummaryBasic](/test/atf_comp/acr_nav.SummaryBasic)|Summary is subset of Screenshot output|
|[acr_nav.ToggleNsDepDetail](/test/atf_comp/acr_nav.ToggleNsDepDetail)|D key toggle: no-op on ctype, activates on namespace header|
|[acr_nav.UnknownKey](/test/atf_comp/acr_nav.UnknownKey)|Unknown key is no-op, does not dismiss startup help|
|[acr_nav.ViewCycle](/test/atf_comp/acr_nav.ViewCycle)|Tab cycling through fields, xref, preview, codegen|
|[acr_nav.ViewToggle](/test/atf_comp/acr_nav.ViewToggle)|Direct viewmode toggle with f/x/c/p keys|
|[acr_nav.ViewportClip](/test/atf_comp/acr_nav.ViewportClip)|Visible* records clipped to terminal viewport|
|[acr_nav.Xref](/test/atf_comp/acr_nav.Xref)|Reverse xref toggle, follow, and go back|
|[acr_nav.XrefEmpty](/test/atf_comp/acr_nav.XrefEmpty)|Reverse xref on ctype with no reverse refs|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Tests -->

