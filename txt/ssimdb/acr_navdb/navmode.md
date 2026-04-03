## acr_navdb.navmode - UI mode for acr_nav


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Navmode

* file:[data/acr_navdb/navmode.ssim](/data/acr_navdb/navmode.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|navmode|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|status_hint|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|""|Status bar hint for this mode|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference acr_navdb.navmode

* [acr_navdb.keybind via navmode](/txt/ssimdb/acr_navdb/keybind.md) - Navigation mode prefix 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FNavmode](/txt/exe/acr_nav/internals.md#acr_nav-fnavmode)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

