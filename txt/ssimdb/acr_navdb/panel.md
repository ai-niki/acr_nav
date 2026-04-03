## acr_navdb.panel - Panel definition for acr_nav TUI layout


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Panel

* file:[data/acr_navdb/panel.ssim](/data/acr_navdb/panel.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|panel|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Display title|
|position|i32|[Val](/txt/exe/amc/reftypes.md#val)||Left-to-right ordering|
|min_width|i32|[Val](/txt/exe/amc/reftypes.md#val)||Minimum column width (0 = derived)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FPanel](/txt/exe/acr_nav/internals.md#acr_nav-fpanel)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

