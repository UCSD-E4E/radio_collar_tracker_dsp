<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.0.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="23" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="balun">
<packages>
<package name="SMA_EDGE_MNT">
<smd name="GND1" x="-2.54" y="0" dx="1.524" dy="5.08" layer="16"/>
<smd name="SIG" x="0" y="0" dx="1.524" dy="5.08" layer="1"/>
<smd name="GND2" x="2.54" y="0" dx="1.524" dy="5.08" layer="16"/>
<wire x1="-3.175" y1="2.54" x2="-3.175" y2="-2.2098" width="0.127" layer="51"/>
<wire x1="-3.175" y1="-2.2098" x2="3.175" y2="-2.2098" width="0.127" layer="51"/>
<wire x1="3.175" y1="-2.2098" x2="3.175" y2="2.54" width="0.127" layer="51"/>
<wire x1="3.175" y1="2.54" x2="2.54" y2="2.54" width="0.127" layer="51"/>
<wire x1="2.54" y1="2.54" x2="-2.54" y2="2.54" width="0.127" layer="51"/>
<wire x1="-2.54" y1="2.54" x2="-3.175" y2="2.54" width="0.127" layer="51"/>
<wire x1="-3.81" y1="2.54" x2="-3.81" y2="-3.81" width="0.127" layer="21"/>
<wire x1="3.81" y1="-3.81" x2="3.81" y2="2.54" width="0.127" layer="21"/>
<smd name="GND3" x="-2.54" y="0" dx="1.524" dy="5.08" layer="1"/>
<smd name="GND4" x="2.54" y="0" dx="1.524" dy="5.08" layer="1"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="5.08" width="0.127" layer="51"/>
<wire x1="-2.54" y1="5.08" x2="2.54" y2="5.08" width="0.127" layer="51"/>
<wire x1="2.54" y1="5.08" x2="2.54" y2="2.54" width="0.127" layer="51"/>
<text x="5.08" y="2.54" size="1.27" layer="25" rot="SR270">&gt;NAME</text>
</package>
</packages>
<symbols>
<symbol name="CONSMA013.062">
<wire x1="0" y1="0" x2="10.16" y2="0" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="20.32" y2="0" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="10.16" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="12.7" y2="5.08" width="0.254" layer="94"/>
<circle x="10.16" y="7.62" radius="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="20.32" y2="7.62" width="0.254" layer="94"/>
<wire x1="20.32" y1="7.62" x2="17.78" y2="10.16" width="0.254" layer="94"/>
<wire x1="20.32" y1="7.62" x2="17.78" y2="5.08" width="0.254" layer="94"/>
<wire x1="20.32" y1="0" x2="17.78" y2="2.54" width="0.254" layer="94"/>
<wire x1="20.32" y1="0" x2="17.78" y2="-2.54" width="0.254" layer="94"/>
<pin name="SIG" x="-5.08" y="7.62" length="middle"/>
<pin name="GND" x="-5.08" y="0" length="middle"/>
<text x="-5.08" y="-5.08" size="1.778" layer="95">&gt;NAME</text>
<text x="-5.08" y="10.16" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="CONSMA013.062_JACK">
<wire x1="-2.54" y1="-2.54" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="10.16" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="-2.54" y1="10.16" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="10.16" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="12.7" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<circle x="10.16" y="7.62" radius="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="20.32" y2="7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="20.32" y2="0" width="0.254" layer="94"/>
<pin name="SIG" x="25.4" y="7.62" length="middle" rot="R180"/>
<pin name="GND" x="25.4" y="0" length="middle" rot="R180"/>
<text x="0" y="-5.08" size="1.778" layer="95">&gt;NAME</text>
<text x="0" y="10.16" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="CONSMA013.062">
<gates>
<gate name="G$1" symbol="CONSMA013.062" x="-10.16" y="0"/>
</gates>
<devices>
<device name="END_MNT" package="SMA_EDGE_MNT">
<connects>
<connect gate="G$1" pin="GND" pad="GND1 GND2 GND3 GND4"/>
<connect gate="G$1" pin="SIG" pad="SIG"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CONSMA013.062_JACK">
<gates>
<gate name="G$1" symbol="CONSMA013.062_JACK" x="-10.16" y="0"/>
</gates>
<devices>
<device name="END_MNT" package="SMA_EDGE_MNT">
<connects>
<connect gate="G$1" pin="GND" pad="GND1 GND2 GND3 GND4"/>
<connect gate="G$1" pin="SIG" pad="SIG"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="rct_lpf_brd">
<packages>
<package name="A01">
<pad name="P3" x="-2.54" y="2.54" drill="0.8"/>
<pad name="P4" x="-2.54" y="-2.54" drill="0.8"/>
<pad name="P6" x="2.54" y="-2.54" drill="0.8"/>
<pad name="P5" x="2.54" y="2.54" drill="0.8"/>
<pad name="P7" x="7.62" y="2.54" drill="0.8"/>
<pad name="P8" x="7.62" y="-2.54" drill="0.8"/>
<pad name="P2" x="-7.62" y="-2.54" drill="0.8"/>
<pad name="P1" x="-7.62" y="2.54" drill="0.8"/>
<wire x1="-10" y1="5" x2="-10" y2="-5" width="0.127" layer="21"/>
<wire x1="-10" y1="-5" x2="10" y2="-5" width="0.127" layer="21"/>
<wire x1="10" y1="-5" x2="10" y2="5" width="0.127" layer="21"/>
<wire x1="10" y1="5" x2="-10" y2="5" width="0.127" layer="21"/>
<text x="-10" y="-6.75" size="1.27" layer="25">&gt;NAME</text>
<text x="-10" y="5.5" size="1.27" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="LPF">
<wire x1="-5.08" y1="5.08" x2="5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="5.08" x2="5.08" y2="-5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="-5.08" x2="-5.08" y2="-5.08" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-5.08" x2="-5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="3.81" y1="-1.778" x2="3.556" y2="-1.778" width="0.1524" layer="94"/>
<wire x1="3.556" y1="-1.778" x2="1.778" y2="-0.508" width="0.1524" layer="94" curve="-71.075356"/>
<wire x1="1.778" y1="-0.508" x2="1.27" y2="0.762" width="0.1524" layer="94" curve="5.75353"/>
<wire x1="1.27" y1="0.762" x2="0.508" y2="2.032" width="0.1524" layer="94" curve="12.571074"/>
<wire x1="0.508" y1="2.032" x2="-0.508" y2="2.54" width="0.1524" layer="94" curve="52.373293"/>
<wire x1="-0.508" y1="2.54" x2="-3.302" y2="2.54" width="0.1524" layer="94"/>
<text x="-5.08" y="6.35" size="1.778" layer="95">&gt;NAME</text>
<text x="-5.08" y="10.16" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="-7.62" visible="pad" length="short" rot="R90"/>
<pin name="RF_IN" x="-7.62" y="2.54" visible="pad" length="short" direction="in"/>
<pin name="RF_OUT" x="7.62" y="2.54" visible="pad" length="short" direction="out" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PLP-250+">
<gates>
<gate name="G$1" symbol="LPF" x="0" y="0"/>
</gates>
<devices>
<device name="" package="A01">
<connects>
<connect gate="G$1" pin="GND" pad="P2 P3 P4 P5 P6 P7"/>
<connect gate="G$1" pin="RF_IN" pad="P1"/>
<connect gate="G$1" pin="RF_OUT" pad="P8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="frames">
<description>&lt;b&gt;Frames for Sheet and Layout&lt;/b&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="FRAME_A_L">
<frame x1="0" y1="0" x2="279.4" y2="215.9" columns="6" rows="5" layer="94" border-bottom="no"/>
</symbol>
<symbol name="DOCFIELD">
<wire x1="0" y1="0" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="87.63" y2="15.24" width="0.1016" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="71.12" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="0" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="87.63" y1="5.08" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="101.6" y1="5.08" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="0" y1="15.24" x2="0" y2="22.86" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="22.86" x2="101.6" y2="15.24" width="0.1016" layer="94"/>
<text x="1.27" y="1.27" size="2.54" layer="94">Date:</text>
<text x="12.7" y="1.27" size="2.54" layer="94">&gt;LAST_DATE_TIME</text>
<text x="72.39" y="1.27" size="2.54" layer="94">Sheet:</text>
<text x="86.36" y="1.27" size="2.54" layer="94">&gt;SHEET</text>
<text x="88.9" y="11.43" size="2.54" layer="94">REV:</text>
<text x="1.27" y="19.05" size="2.54" layer="94">TITLE:</text>
<text x="1.27" y="11.43" size="2.54" layer="94">Document Number:</text>
<text x="17.78" y="19.05" size="2.54" layer="94">&gt;DRAWING_NAME</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="FRAME_A_L" prefix="FRAME" uservalue="yes">
<description>&lt;b&gt;FRAME&lt;/b&gt; A Size , 8 1/2 x 11 INCH, Landscape&lt;p&gt;</description>
<gates>
<gate name="G$1" symbol="FRAME_A_L" x="0" y="0" addlevel="always"/>
<gate name="G$2" symbol="DOCFIELD" x="172.72" y="0" addlevel="always"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="SMA_OUT" library="balun" deviceset="CONSMA013.062" device="END_MNT"/>
<part name="SMA_IN" library="balun" deviceset="CONSMA013.062_JACK" device="END_MNT"/>
<part name="LPF" library="rct_lpf_brd" deviceset="PLP-250+" device=""/>
<part name="FRAME1" library="frames" deviceset="FRAME_A_L" device="" value="1"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="SMA_OUT" gate="G$1" x="111.76" y="114.3"/>
<instance part="SMA_IN" gate="G$1" x="30.48" y="114.3"/>
<instance part="LPF" gate="G$1" x="81.28" y="119.38"/>
<instance part="FRAME1" gate="G$1" x="0" y="0"/>
<instance part="FRAME1" gate="G$2" x="172.72" y="0"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="SMA_IN" gate="G$1" pin="SIG"/>
<pinref part="LPF" gate="G$1" pin="RF_IN"/>
<wire x1="55.88" y1="121.92" x2="73.66" y2="121.92" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="LPF" gate="G$1" pin="RF_OUT"/>
<pinref part="SMA_OUT" gate="G$1" pin="SIG"/>
<wire x1="88.9" y1="121.92" x2="106.68" y2="121.92" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="LPF" gate="G$1" pin="GND"/>
<wire x1="81.28" y1="111.76" x2="81.28" y2="106.68" width="0.1524" layer="91"/>
<pinref part="SMA_OUT" gate="G$1" pin="GND"/>
<wire x1="81.28" y1="106.68" x2="106.68" y2="106.68" width="0.1524" layer="91"/>
<wire x1="106.68" y1="106.68" x2="106.68" y2="114.3" width="0.1524" layer="91"/>
<pinref part="SMA_IN" gate="G$1" pin="GND"/>
<wire x1="81.28" y1="106.68" x2="55.88" y2="106.68" width="0.1524" layer="91"/>
<wire x1="55.88" y1="106.68" x2="55.88" y2="114.3" width="0.1524" layer="91"/>
<junction x="81.28" y="106.68"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
