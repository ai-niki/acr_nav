## acr_navdb.reftypestyle - Mapping from reftype to navstyle for field coloring


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)acr_navdb.Reftypestyle

* file:[data/acr_navdb/reftypestyle.ssim](/data/acr_navdb/reftypestyle.ssim)

italicised fields: *reftype, navstyle* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reftypestyle|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*reftype*|*[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of reftypestyle*|
|*navstyle*|*[acr_navdb.Navstyle](/txt/ssimdb/acr_navdb/navstyle.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of reftypestyle*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_nav](/txt/exe/acr_nav/internals.md) as [acr_nav.FReftypestyle](/txt/exe/acr_nav/internals.md#acr_nav-freftypestyle)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

