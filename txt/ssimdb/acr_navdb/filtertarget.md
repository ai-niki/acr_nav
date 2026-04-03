## acr_navdb.filtertarget -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Filtertarget

* file:[data/acr_navdb/filtertarget.ssim](/data/acr_navdb/filtertarget.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|filtertarget|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|label|[algo.Smallstr16](/txt/protocol/algo/README.md#algo-smallstr16)|[Val](/txt/exe/amc/reftypes.md#val)||Status bar indicator prefix|
|next|[acr_navdb.Filtertarget](/txt/ssimdb/acr_navdb/filtertarget.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Next filtertarget in Tab cycle|
|description|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Hint shown in filter mode status bar|
|match_ctype_name|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Match against ctype name|
|match_field_name|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Match against field name|
|match_comment|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Match against field comment|
|match_arg|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Match against field argument type|
|match_reftype|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Match against field reftype|
|has_field_criteria|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Target searches field-level properties (enables right-panel highlighting)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference acr_navdb.filtertarget

* [acr_navdb.filtertarget via next](/txt/ssimdb/acr_navdb/filtertarget.md) - Next filtertarget in Tab cycle 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FFiltertarget](/txt/exe/acr_nav/internals.md#acr_nav-ffiltertarget)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

