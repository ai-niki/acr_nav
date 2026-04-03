## acr_navdb.navstyle - Terminal visual style for acr_nav UI elements


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Navstyle

* file:[data/acr_navdb/navstyle.ssim](/data/acr_navdb/navstyle.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|navstyle|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|bold|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|dim|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|reverse|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|fg_color|[algo.TermColor](/txt/protocol/algo/README.md#algo-termcolor)|[Val](/txt/exe/amc/reftypes.md#val)||Foreground color|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference acr_navdb.navstyle

* [acr_navdb.reftypestyle via navstyle](/txt/ssimdb/acr_navdb/reftypestyle.md) - Mapping from reftype to navstyle for field coloring 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FNavstyle](/txt/exe/acr_nav/internals.md#acr_nav-fnavstyle)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

