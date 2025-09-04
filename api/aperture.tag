<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>CLAUDE.md</name>
    <path>/home/runner/work/aperture/aperture/</path>
    <filename>da/ddc/CLAUDE_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>DESIGN.md</name>
    <path>/home/runner/work/aperture/aperture/docs/</path>
    <filename>d7/d9a/DESIGN_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>fluent_api.hpp</name>
    <path>/home/runner/work/aperture/aperture/</path>
    <filename>d5/df8/fluent__api_8hpp.html</filename>
    <class kind="class">aperture::Computation</class>
    <class kind="class">aperture::Expr</class>
    <class kind="class">aperture::Hole</class>
    <class kind="class">aperture::ops::LambdaBuilder</class>
    <class kind="class">aperture::SecureComputation</class>
    <namespace>aperture</namespace>
    <namespace>aperture::build</namespace>
    <namespace>aperture::ops</namespace>
  </compound>
  <compound kind="file">
    <name>README.md</name>
    <path>/home/runner/work/aperture/aperture/</path>
    <filename>da/ddd/README_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>test_framework.hpp</name>
    <path>/home/runner/work/aperture/aperture/</path>
    <filename>df/d34/test__framework_8hpp.html</filename>
    <class kind="struct">secure::test::PartialEvalTester::HoleTest</class>
    <class kind="class">secure::test::PartialEvalTester</class>
    <class kind="struct">secure::test::TestRunner::TestCase</class>
    <class kind="class">secure::test::TestRunner</class>
    <namespace>secure</namespace>
    <namespace>secure::test</namespace>
  </compound>
  <compound kind="class">
    <name>aperture::Computation</name>
    <filename>d2/d59/classaperture_1_1Computation.html</filename>
    <member kind="function">
      <type></type>
      <name>Computation</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a5c7301441239d654c0735b78e7900a25</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Computation</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a0996ab93cef91bbde36e7758dac016ae</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eval</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a36fa86f5fa7fd544b31000839fcc5f38</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>eval_expr</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>abbdb0ec8852182bd3ae633b2d8707f90</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>fill</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>abe8394ba170e25e58bbe4778c94c649a</anchor>
      <arglist>(const std::string &amp;name, double value, Args... args)</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>fill</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a492afba43f605f5d8e26afc9eead26a4</anchor>
      <arglist>(const std::unordered_map&lt; std::string, double &gt; &amp;values)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>holes</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a6d1abf0a3860f2153c0b64115e3bffea</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>partial_eval</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>ae6996ff6cfb2ab35ade189bc6b925d96</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>to_string</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a2bb28574bba9ef07b79d9db98aef83ba</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>with</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a18901e130b9bc81f9547b70a54e7e66f</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>secure::Evaluator</type>
      <name>evaluator</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a2f92e6cc2f25d6a58697581ac970e1b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>expression</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a89d01f6289f32a7d19b76dec03203428</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>fill_impl</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a0e884dcb98816f301136ae007baa670a</anchor>
      <arglist>(std::unordered_map&lt; std::string, secure::ValuePtr &gt; &amp;values, const std::string &amp;name, double value, Args... args)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>aperture::Expr</name>
    <filename>d9/ddc/classaperture_1_1Expr.html</filename>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>ac26bb8edfdda81c4123a6e7b125f1fc0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a50556721f198b8a87374cc53a77a540f</anchor>
      <arglist>(const char *s)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>acea56b2d2c8048b1c194878370b337e5</anchor>
      <arglist>(const std::string &amp;s)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a518d566e5cacdf7cfee25859af0540ce</anchor>
      <arglist>(double n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a6de8c67cc70a162cde3fda2f89c9fa97</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Expr</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a78175dcc37ec4017db70271fa69066dc</anchor>
      <arglist>(secure::ValuePtr v)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>get_holes</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>ad8938d2552a03de773141889098d4ded</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>secure::ValuePtr</type>
      <name>get_value</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a209381a5628dff4606c7db5a5d0e9b0d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>has_holes</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a75c61eb466ef4b5589cc9ef59650eb01</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator()</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a9ec3df0de3a5667605ae6b23587dee12</anchor>
      <arglist>(Args &amp;&amp;... args) const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator*</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>ac4b6132514572ab4b6859785125ef5e5</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator*=</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a5ef1b70cda35d361c4c943c29c8f647b</anchor>
      <arglist>(const Expr &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator+</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a02be101a980c4364da1a1ac707cfeaaa</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr &amp;</type>
      <name>operator+=</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>abb9f78111cc48e8e13ce844c71245989</anchor>
      <arglist>(const Expr &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator-</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>aabd7876fa4fc124850f826baa510d625</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator-</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a85c4d283a965dc9870fbd259ecb93c49</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator/</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>ac69cdfe5390c4fa2861e293adb23d36a</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator&lt;</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a669180e27c924bb9c7d060684b2d6adc</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator==</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a3cc898dd5e40f7ce76f703ce712946a9</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator&gt;</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a10c1f562904697cfca06c6b0bfb98bfb</anchor>
      <arglist>(const Expr &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>to_string</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>adcea8cd2ac6a29432d7e47ed5d612e05</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Expr</type>
      <name>hole</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a08e3571795fd516aae9939d952f43bae</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>secure::ValuePtr</type>
      <name>val</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>acedf71dbeb82ca546c8e829fc183180a</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>Computation</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a8412abda57ded9cbbc7067b335143d7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>SecureComputation</name>
      <anchorfile>d9/ddc/classaperture_1_1Expr.html</anchorfile>
      <anchor>a0151359fcda013fe919d5d35a02327fe</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>aperture::Hole</name>
    <filename>d6/df5/classaperture_1_1Hole.html</filename>
    <templarg>typename T</templarg>
    <member kind="function">
      <type></type>
      <name>Hole</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>a362c11a3b0a084c1c2ed33cd9069f21e</anchor>
      <arglist>(std::string n)</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>get_name</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>a3a737600ab8daadba485bb48e360678e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>get_value</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>a7b70515730487db8ccd95e4704895a26</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_filled</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>af81edfe44136a299ba4ce220b61152f8</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>operator Expr</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>a2e1f11aa96bef6ccc2048cbaa86c4c95</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Hole &amp;</type>
      <name>operator=</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>adfe625b1e16fde9c917516f099573864</anchor>
      <arglist>(T val)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>filled</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>aa26f522758343a2d51635293a824c154</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>a14774d9c5ed3eb668aebdb010f5aa012</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>T</type>
      <name>value</name>
      <anchorfile>d6/df5/classaperture_1_1Hole.html</anchorfile>
      <anchor>abe61aa90a503b4f4cab165d3b24d4e58</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>secure::test::PartialEvalTester::HoleTest</name>
    <filename>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</filename>
    <member kind="variable">
      <type>std::string</type>
      <name>expected_partial</name>
      <anchorfile>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</anchorfile>
      <anchor>ad0157adbda0a637d8b1e450436a3eab3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>expected_result</name>
      <anchorfile>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</anchorfile>
      <anchor>a03b49f2447aeb7d6f2db5031a41404e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>expr_with_holes</name>
      <anchorfile>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</anchorfile>
      <anchor>a83cbda2074ccb4ccb1862b0cfc554296</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::unordered_map&lt; std::string, double &gt;</type>
      <name>hole_values</name>
      <anchorfile>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</anchorfile>
      <anchor>a1aef1676ea8ab324e8bf3884bc96d136</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>d9/da8/structsecure_1_1test_1_1PartialEvalTester_1_1HoleTest.html</anchorfile>
      <anchor>addb96d23c1490ba77f2ea90f32da6af0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>aperture::ops::LambdaBuilder</name>
    <filename>d9/d32/classaperture_1_1ops_1_1LambdaBuilder.html</filename>
    <templarg>typename F</templarg>
    <member kind="function">
      <type></type>
      <name>LambdaBuilder</name>
      <anchorfile>d9/d32/classaperture_1_1ops_1_1LambdaBuilder.html</anchorfile>
      <anchor>a70acec01b103f49b8b3dcc113db83203</anchor>
      <arglist>(std::initializer_list&lt; std::string &gt; p)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>operator[]</name>
      <anchorfile>d9/d32/classaperture_1_1ops_1_1LambdaBuilder.html</anchorfile>
      <anchor>a1990e4ec7acbe24cde26113e3021cffe</anchor>
      <arglist>(F body_fn) const</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; std::string &gt;</type>
      <name>params</name>
      <anchorfile>d9/d32/classaperture_1_1ops_1_1LambdaBuilder.html</anchorfile>
      <anchor>a9fd9619d58576fdaf1c260a0dcd8abcb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>secure::test::PartialEvalTester</name>
    <filename>df/da9/classsecure_1_1test_1_1PartialEvalTester.html</filename>
    <class kind="struct">secure::test::PartialEvalTester::HoleTest</class>
    <member kind="function">
      <type>bool</type>
      <name>test_partial_eval</name>
      <anchorfile>df/da9/classsecure_1_1test_1_1PartialEvalTester.html</anchorfile>
      <anchor>a062463c83b34aa6bbd8e9254f0d3fe4a</anchor>
      <arglist>(const HoleTest &amp;test, bool verbose=false)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>Evaluator</type>
      <name>eval</name>
      <anchorfile>df/da9/classsecure_1_1test_1_1PartialEvalTester.html</anchorfile>
      <anchor>a6ded8ab36bf301665494f299801f154d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>aperture::SecureComputation</name>
    <filename>db/dd3/classaperture_1_1SecureComputation.html</filename>
    <base>aperture::Computation</base>
    <member kind="function">
      <type>SecureComputation &amp;</type>
      <name>allow_partial_eval</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>acc6655ee85cbd6a1c2647ab463ccd048</anchor>
      <arglist>(bool allow)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Computation</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a5c7301441239d654c0735b78e7900a25</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Computation</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a0996ab93cef91bbde36e7758dac016ae</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eval</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a36fa86f5fa7fd544b31000839fcc5f38</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>eval_expr</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>abbdb0ec8852182bd3ae633b2d8707f90</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>fill</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>abe8394ba170e25e58bbe4778c94c649a</anchor>
      <arglist>(const std::string &amp;name, double value, Args... args)</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>fill</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a492afba43f605f5d8e26afc9eead26a4</anchor>
      <arglist>(const std::unordered_map&lt; std::string, double &gt; &amp;values)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>holes</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a6d1abf0a3860f2153c0b64115e3bffea</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>partial_eval</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>ae6996ff6cfb2ab35ade189bc6b925d96</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>secure_compute</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a62641e690ce3262135ed09503948f6f6</anchor>
      <arglist>(const std::unordered_map&lt; std::string, double &gt; &amp;private_data)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>send_to_server</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a90f59528e017955ac27d1e5c8b861787</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>to_string</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a2bb28574bba9ef07b79d9db98aef83ba</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>verify</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a28658a988691662802dd3f134d56f89c</anchor>
      <arglist>(const Expr &amp;received)</arglist>
    </member>
    <member kind="function">
      <type>Computation &amp;</type>
      <name>with</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a18901e130b9bc81f9547b70a54e7e66f</anchor>
      <arglist>(const Expr &amp;expr)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>secure::Evaluator</type>
      <name>evaluator</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a2f92e6cc2f25d6a58697581ac970e1b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Expr</type>
      <name>expression</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a89d01f6289f32a7d19b76dec03203428</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>fill_impl</name>
      <anchorfile>d2/d59/classaperture_1_1Computation.html</anchorfile>
      <anchor>a0e884dcb98816f301136ae007baa670a</anchor>
      <arglist>(std::unordered_map&lt; std::string, secure::ValuePtr &gt; &amp;values, const std::string &amp;name, double value, Args... args)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>allow_partial</name>
      <anchorfile>db/dd3/classaperture_1_1SecureComputation.html</anchorfile>
      <anchor>a4f9516caa2022b95d24b11a1f4554139</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>secure::test::TestRunner::TestCase</name>
    <filename>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</filename>
    <member kind="variable">
      <type>std::string</type>
      <name>aperture_code</name>
      <anchorfile>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</anchorfile>
      <anchor>a31453205c53955bb6a1a2966bbd05714</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>expected_result</name>
      <anchorfile>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</anchorfile>
      <anchor>a198a0c4f24355d40fd70a6611eac17f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</anchorfile>
      <anchor>a3e8aff6831655b11f8272da00f03a798</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>should_error</name>
      <anchorfile>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</anchorfile>
      <anchor>af47905810e5d681c6186f4f65d92c218</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>tolerance</name>
      <anchorfile>df/dec/structsecure_1_1test_1_1TestRunner_1_1TestCase.html</anchorfile>
      <anchor>a03fc216ad07b5b2e3463133dca82126f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>secure::test::TestRunner</name>
    <filename>d2/d78/classsecure_1_1test_1_1TestRunner.html</filename>
    <class kind="struct">secure::test::TestRunner::TestCase</class>
    <member kind="function">
      <type></type>
      <name>TestRunner</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>ae5869e19fdd97c46dd1455843ff50a20</anchor>
      <arglist>(bool verbose=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_error_test</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>a3df19389350fa97007b18dc01dcf598c</anchor>
      <arglist>(const std::string &amp;name, const std::string &amp;aperture_code)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_test</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>abcd5f998f5f2f77967b388d82e7037b6</anchor>
      <arglist>(const std::string &amp;name, const std::string &amp;aperture_code, double expected, double tolerance=1e-9)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>all_passed</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>aca5967a5a652ac11283b0844c1a8354d</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>run_all</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>a94e4116c281cf6841d6296f65ffdbf4b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>run_test</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>a4e679cb16b833a442dbca329562002db</anchor>
      <arglist>(const TestCase &amp;test)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>failed</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>a509199775a3985a9f6b254887c8dc7c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>passed</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>aca6a45885eb6bc7a9097278ef4d4dca0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::vector&lt; TestCase &gt;</type>
      <name>tests</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>a02240737f4c319cbc16010f920ba725c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>verbose</name>
      <anchorfile>d2/d78/classsecure_1_1test_1_1TestRunner.html</anchorfile>
      <anchor>aa96e255ccf9c68e0f7e9217f48ffa41c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>aperture</name>
    <filename>d5/dca/namespaceaperture.html</filename>
    <namespace>aperture::build</namespace>
    <namespace>aperture::ops</namespace>
    <class kind="class">aperture::Computation</class>
    <class kind="class">aperture::Expr</class>
    <class kind="class">aperture::Hole</class>
    <class kind="class">aperture::SecureComputation</class>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>d5/dca/namespaceaperture.html</anchorfile>
      <anchor>a19c3bd2e6b9898d1f6d91c5e11e7dd6d</anchor>
      <arglist>(std::ostream &amp;os, const Expr &amp;expr)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>aperture::build</name>
    <filename>dc/d1b/namespaceaperture_1_1build.html</filename>
    <member kind="function">
      <type>Expr</type>
      <name>hole</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>a6a50c75c9bd356f7389b861115a53f80</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>num</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>a65b4207ecbcf6370a69baad6d26ae2e6</anchor>
      <arglist>(double n)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>sym</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>a4ccc0a2913956a87f7a1f8381624803b</anchor>
      <arglist>(const std::string &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>x</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>ad5ad3a520e632a4547720c931b53ad68</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>y</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>a840a8a34752dac04242b1ca919af1ede</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>z</name>
      <anchorfile>dc/d1b/namespaceaperture_1_1build.html</anchorfile>
      <anchor>af3ce86ababe360c2bfd17cc68bffa553</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>aperture::ops</name>
    <filename>d9/dec/namespaceaperture_1_1ops.html</filename>
    <class kind="class">aperture::ops::LambdaBuilder</class>
    <member kind="function">
      <type>Expr</type>
      <name>if_then_else</name>
      <anchorfile>d9/dec/namespaceaperture_1_1ops.html</anchorfile>
      <anchor>a21851a6150c640b738cfb729cce6fe85</anchor>
      <arglist>(const Expr &amp;cond, const Expr &amp;then_branch, const Expr &amp;else_branch)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>lambda</name>
      <anchorfile>d9/dec/namespaceaperture_1_1ops.html</anchorfile>
      <anchor>acafc8da9a86ab6bb1832668efe30e617</anchor>
      <arglist>(const std::string &amp;param, std::function&lt; Expr(Expr)&gt; body)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>lambda</name>
      <anchorfile>d9/dec/namespaceaperture_1_1ops.html</anchorfile>
      <anchor>ae8352b54472de5a2798f4e40c7faf667</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;params, std::function&lt; Expr(std::vector&lt; Expr &gt;)&gt; body)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>let</name>
      <anchorfile>d9/dec/namespaceaperture_1_1ops.html</anchorfile>
      <anchor>aebc5ba1a632cb9f8f31814cc3731ecfa</anchor>
      <arglist>(const std::string &amp;var, const Expr &amp;value, const Expr &amp;body)</arglist>
    </member>
    <member kind="function">
      <type>Expr</type>
      <name>let</name>
      <anchorfile>d9/dec/namespaceaperture_1_1ops.html</anchorfile>
      <anchor>a062f8920ee3752e1cfa47bc050d9d1b5</anchor>
      <arglist>(const std::vector&lt; std::pair&lt; std::string, Expr &gt; &gt; &amp;bindings, const Expr &amp;body)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>secure</name>
    <filename>df/dc1/namespacesecure.html</filename>
    <namespace>secure::test</namespace>
  </compound>
  <compound kind="namespace">
    <name>secure::test</name>
    <filename>dd/d5a/namespacesecure_1_1test.html</filename>
    <class kind="class">secure::test::PartialEvalTester</class>
    <class kind="class">secure::test::TestRunner</class>
  </compound>
  <compound kind="page">
    <name>md__2home_2runner_2work_2aperture_2aperture_2README</name>
    <title>Secure Aperture Language</title>
    <filename>d1/d6f/md__2home_2runner_2work_2aperture_2aperture_2README.html</filename>
  </compound>
  <compound kind="page">
    <name>md__2home_2runner_2work_2aperture_2aperture_2CLAUDE</name>
    <title>CLAUDE.md</title>
    <filename>dc/d4a/md__2home_2runner_2work_2aperture_2aperture_2CLAUDE.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Aperture Language Design Document</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Aperture Language Design Document">md__2home_2runner_2work_2aperture_2aperture_2docs_2DESIGN</docanchor>
  </compound>
</tagfile>
