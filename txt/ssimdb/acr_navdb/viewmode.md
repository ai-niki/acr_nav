## acr_navdb.viewmode - Right-panel view mode for acr_nav


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Viewmode

* file:[data/acr_navdb/viewmode.ssim](/data/acr_navdb/viewmode.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|title|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Right panel title for this viewmode|
|next|[acr_navdb.Viewmode](/txt/ssimdb/acr_navdb/viewmode.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Next viewmode in Tab cycle|
|empty_msg|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Message shown when right panel has no items|
|has_fields|bool|[Val](/txt/exe/amc/reftypes.md#val)||Y: renders field records; N: renders preformatted lines|
|is_overlay|bool|[Val](/txt/exe/amc/reftypes.md#val)||Y: overlay viewmode (help, detail); suppresses need_no_overlay hints|
|need_ssimfile|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Viewmode requires ssimfile-backed ctype|
|is_reverse|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Y=reverse xrefs, N=forward fields|
|status_hint|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|""|Status bar hint (right panel for views, full hint for overlays)|
|scope_ns|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Y: viewmode is scoped to namespace (auto-activate/deactivate on ns headers)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference acr_navdb.viewmode

* [acr_navdb.navaction via target_viewmode](/txt/ssimdb/acr_navdb/navaction.md) - Viewmode to toggle (empty=not a toggle action) 
* [acr_navdb.viewmode via next](/txt/ssimdb/acr_navdb/viewmode.md) - Next viewmode in Tab cycle 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FViewmode](/txt/exe/acr_nav/internals.md#acr_nav-fviewmode)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

