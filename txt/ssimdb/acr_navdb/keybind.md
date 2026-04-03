## acr_navdb.keybind - Key-to-action mapping for acr_nav


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Keybind

* file:[data/acr_navdb/keybind.ssim](/data/acr_navdb/keybind.ssim)

italicised fields: *navmode, key* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|keybind|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|navaction|[acr_navdb.Navaction](/txt/ssimdb/acr_navdb/navaction.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Navigation action|
|*navmode*|*[acr_navdb.Navmode](/txt/ssimdb/acr_navdb/navmode.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Navigation mode prefix<br>.RL of keybind*|
|*key*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Key name portion<br>.RR of keybind*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FKeybind](/txt/exe/acr_nav/internals.md#acr_nav-fkeybind)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

