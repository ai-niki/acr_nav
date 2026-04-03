## acr_navdb.navaction - Controlled vocabulary of navigation actions


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Navaction

* file:[data/acr_navdb/navaction.ssim](/data/acr_navdb/navaction.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|navaction|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|helpgroup|[acr_navdb.Helpgroup](/txt/ssimdb/acr_navdb/helpgroup.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Help group; empty=hidden from help|
|sort_order|i32|[Val](/txt/exe/amc/reftypes.md#val)||Sort order within help group|
|passive|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Movement-only action; does not dismiss startup help|
|need_no_overlay|bool|[Val](/txt/exe/amc/reftypes.md#val)||Hint hidden when viewmode.is_overlay is Y|
|dismiss_viewmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Viewmode this action dismisses; empty means any overlay|
|target_viewmode|[acr_navdb.Viewmode](/txt/ssimdb/acr_navdb/viewmode.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Viewmode to toggle (empty=not a toggle action)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference acr_navdb.navaction

* [acr_navdb.keybind via navaction](/txt/ssimdb/acr_navdb/keybind.md) - Navigation action 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FNavaction](/txt/exe/acr_nav/internals.md#acr_nav-fnavaction)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

