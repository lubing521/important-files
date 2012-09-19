#include "stdafx.h"
#include "ConfigBusiness.h"

#include "../../../../include/Utility/utility.h"
#include <map>


namespace i8desk
{
	namespace business
	{
		namespace config
		{
			struct AreaInfo
			{
				size_t ParentID;
				size_t SelfID;
				TCHAR Name[MAX_PATH];

				AreaInfo()
				{
					memset(this, 0, sizeof(*this));
				}
				template < typename T, size_t N >
				AreaInfo(size_t parentID, size_t selfID, T (&arr)[N])
					: ParentID(parentID)
					, SelfID(selfID)
				{
					utility::Strcpy(Name, arr);
				}
			};

			typedef std::map<size_t, AreaInfo> ProvinceInfo;
			typedef std::map<size_t, AreaInfo> CityInfo;
			ProvinceInfo Provience;
			CityInfo City;

			static struct InitArea
			{
				InitArea()
				{
					Provience[110000] = AreaInfo(0, 110000, _T("北京"));
					Provience[500000] = AreaInfo(0, 500000, _T("重庆"));
					Provience[350000] = AreaInfo(0, 350000, _T("福建"));
					Provience[620000] = AreaInfo(0, 620000, _T("甘肃"));
					Provience[440000] = AreaInfo(0, 440000, _T("广东"));
					Provience[450000] = AreaInfo(0, 450000, _T("广西"));
					Provience[520000] = AreaInfo(0, 520000, _T("贵州"));
					Provience[460000] = AreaInfo(0, 460000, _T("海南"));
					Provience[130000] = AreaInfo(0, 130000, _T("河北"));
					Provience[410000] = AreaInfo(0, 410000, _T("河南"));
					Provience[230000] = AreaInfo(0, 230000, _T("黑龙江"));
					Provience[420000] = AreaInfo(0, 420000, _T("湖北"));
					Provience[430000] = AreaInfo(0, 430000, _T("湖南"));
					Provience[220000] = AreaInfo(0, 220000, _T("吉林"));
					Provience[320000] = AreaInfo(0, 320000, _T("江苏"));
					Provience[360000] = AreaInfo(0, 360000, _T("江西"));
					Provience[210000] = AreaInfo(0, 210000, _T("辽宁"));
					Provience[150000] = AreaInfo(0, 150000, _T("内蒙古"));
					Provience[640000] = AreaInfo(0, 640000, _T("宁夏"));
					Provience[630000] = AreaInfo(0, 630000, _T("青海"));
					Provience[370000] = AreaInfo(0, 370000, _T("山东"));
					Provience[140000] = AreaInfo(0, 140000, _T("山西"));
					Provience[610000] = AreaInfo(0, 610000, _T("陕西"));
					Provience[310000] = AreaInfo(0, 310000, _T("上海"));
					Provience[510000] = AreaInfo(0, 510000, _T("四川"));
					Provience[120000] = AreaInfo(0, 120000, _T("天津"));
					Provience[540000] = AreaInfo(0, 540000, _T("西藏"));
					Provience[650000] = AreaInfo(0, 650000, _T("新疆"));
					Provience[530000] = AreaInfo(0, 530000, _T("云南"));
					Provience[330000] = AreaInfo(0, 330000, _T("浙江"));
					Provience[340000] = AreaInfo(0, 340000, _T("安徽"));
					Provience[1000000] = AreaInfo(0, 1000000, _T("国外"));
					Provience[2000000] = AreaInfo(0, 2000000, _T("OEM专用区域"));


					//---ws

					City[230600] = AreaInfo(230000, 230600, _T("大庆"));
					City[232700] = AreaInfo(230000, 232700, _T("大兴安岭"));
					City[230100] = AreaInfo(230000, 230100, _T("哈尔滨"));
					City[230400] = AreaInfo(230000, 230400, _T("鹤岗"));
					City[231100] = AreaInfo(230000, 231100, _T("黑河"));
					City[230300] = AreaInfo(230000, 230300, _T("鸡西"));
					City[230800] = AreaInfo(230000, 230800, _T("佳木斯"));
					City[231000] = AreaInfo(230000, 231000, _T("牡丹江"));
					City[230900] = AreaInfo(230000, 230900, _T("七台河"));
					City[230200] = AreaInfo(230000, 230200, _T("齐齐哈尔"));
					City[230500] = AreaInfo(230000, 230500, _T("双鸭山"));
					City[231200] = AreaInfo(230000, 231200, _T("绥化"));
					City[230700] = AreaInfo(230000, 230700, _T("伊春"));
					City[420700] = AreaInfo(420000, 420700, _T("鄂州"));
					City[422800] = AreaInfo(420000, 422800, _T("恩施土家族苗族自治州"));
					City[421100] = AreaInfo(420000, 421100, _T("黄冈"));
					City[420200] = AreaInfo(420000, 420200, _T("黄石"));
					City[420800] = AreaInfo(420000, 420800, _T("荆门"));
					City[421000] = AreaInfo(420000, 421000, _T("荆州"));
					City[429000] = AreaInfo(420000, 429000, _T("省直辖"));
					City[420200] = AreaInfo(420000, 420200, _T("黄石"));
					City[420800] = AreaInfo(420000, 420800, _T("荆门"));
					City[421000] = AreaInfo(420000, 421000, _T("荆州"));
					City[429000] = AreaInfo(420000, 429000, _T("省直辖"));
					City[420300] = AreaInfo(420000, 420300, _T("十堰"));
					City[421300] = AreaInfo(420000, 421300, _T("随州"));
					City[420100] = AreaInfo(420000, 420100, _T("武汉"));
					City[421200] = AreaInfo(420000, 421200, _T("咸宁"));
					City[420600] = AreaInfo(420000, 420600, _T("襄樊"));
					City[420900] = AreaInfo(420000, 420900, _T("孝感"));
					City[420500] = AreaInfo(420000, 420500, _T("宜昌"));
					City[430700] = AreaInfo(430000, 430700, _T("常德"));
					City[430100] = AreaInfo(430000, 430100, _T("长沙"));
					City[431000] = AreaInfo(430000, 431000, _T("郴州"));
					City[430400] = AreaInfo(430000, 430400, _T("衡阳"));
					City[431200] = AreaInfo(430000, 431200, _T("怀化"));
					City[431300] = AreaInfo(430000, 431300, _T("娄底"));
					City[430500] = AreaInfo(430000, 430500, _T("邵阳"));
					City[430300] = AreaInfo(430000, 430300, _T("湘潭"));
					City[433100] = AreaInfo(430000, 433100, _T("湘西土家族苗族自治州"));
					City[430900] = AreaInfo(430000, 430900, _T("益阳"));
					City[431100] = AreaInfo(430000, 431100, _T("永州"));
					City[430600] = AreaInfo(430000, 430600, _T("岳阳"));
					City[430800] = AreaInfo(430000, 430800, _T("张家界"));
					City[430200] = AreaInfo(430000, 430200, _T("株洲"));
					City[220800] = AreaInfo(220000, 220800, _T("白城"));
					City[220600] = AreaInfo(220000, 220600, _T("白山"));
					City[220100] = AreaInfo(220000, 220100, _T("长春"));
					City[220200] = AreaInfo(220000, 220200, _T("吉林"));
					City[220400] = AreaInfo(220000, 220400, _T("辽源"));
					City[220300] = AreaInfo(220000, 220300, _T("四平"));
					City[220700] = AreaInfo(220000, 220700, _T("松原"));
					City[220500] = AreaInfo(220000, 220500, _T("通化"));
					City[222400] = AreaInfo(220000, 222400, _T("延边朝鲜族自治州"));
					City[320400] = AreaInfo(320000, 320400, _T("常州"));
					City[320800] = AreaInfo(320000, 320800, _T("淮安"));
					City[320700] = AreaInfo(320000, 320700, _T("连云港"));
					City[320100] = AreaInfo(320000, 320100, _T("南京"));
					City[320600] = AreaInfo(320000, 320600, _T("南通"));
					City[320500] = AreaInfo(320000, 320500, _T("苏州"));
					City[321300] = AreaInfo(320000, 321300, _T("宿迁"));
					City[321200] = AreaInfo(320000, 321200, _T("泰州"));
					City[320200] = AreaInfo(320000, 320200, _T("无锡"));
					City[320300] = AreaInfo(320000, 320300, _T("徐州"));
					City[320900] = AreaInfo(320000, 320900, _T("盐城"));
					City[321000] = AreaInfo(320000, 321000, _T("扬州"));
					City[321100] = AreaInfo(320000, 321100, _T("镇江"));
					City[361000] = AreaInfo(360000, 361000, _T("抚州"));
					City[360700] = AreaInfo(360000, 360700, _T("赣州"));
					City[360800] = AreaInfo(360000, 360800, _T("吉安"));
					City[360200] = AreaInfo(360000, 360200, _T("景德镇"));
					City[360400] = AreaInfo(360000, 360400, _T("九江"));
					City[360100] = AreaInfo(360000, 360100, _T("南昌"));
					City[360300] = AreaInfo(360000, 360300, _T("萍乡"));
					City[361100] = AreaInfo(360000, 361100, _T("上饶"));
					City[360500] = AreaInfo(360000, 360500, _T("新余"));
					City[360900] = AreaInfo(360000, 360900, _T("宜春"));
					City[360600] = AreaInfo(360000, 360600, _T("鹰潭"));
					City[210300] = AreaInfo(210000, 210300, _T("鞍山"));
					City[210500] = AreaInfo(210000, 210500, _T("本溪"));
					City[211300] = AreaInfo(210000, 211300, _T("朝阳"));
					City[210200] = AreaInfo(210000, 210200, _T("大连"));
					City[210600] = AreaInfo(210000, 210600, _T("丹东"));
					City[210400] = AreaInfo(210000, 210400, _T("抚顺"));
					City[210900] = AreaInfo(210000, 210900, _T("阜新"));
					City[211400] = AreaInfo(210000, 211400, _T("葫芦岛"));
					City[210700] = AreaInfo(210000, 210700, _T("锦州"));
					City[211000] = AreaInfo(210000, 211000, _T("辽阳"));
					City[211100] = AreaInfo(210000, 211100, _T("盘锦"));
					City[210100] = AreaInfo(210000, 210100, _T("沈阳"));
					City[211200] = AreaInfo(210000, 211200, _T("铁岭"));
					City[210800] = AreaInfo(210000, 210800, _T("营口"));
					City[152900] = AreaInfo(150000, 152900, _T("阿拉善盟"));
					City[150800] = AreaInfo(150000, 150800, _T("巴彦淖尔盟"));
					City[150200] = AreaInfo(150000, 150200, _T("包头"));
					City[150400] = AreaInfo(150000, 150400, _T("赤峰"));
					City[150600] = AreaInfo(150000, 150600, _T("鄂尔多斯"));
					City[150100] = AreaInfo(150000, 150100, _T("呼和浩特"));
					City[150700] = AreaInfo(150000, 150700, _T("呼伦贝尔"));
					City[150500] = AreaInfo(150000, 150500, _T("通辽"));
					City[150300] = AreaInfo(150000, 150300, _T("乌海"));
					City[150900] = AreaInfo(150000, 150900, _T("乌兰察布盟"));
					City[152500] = AreaInfo(150000, 152500, _T("锡林郭勒盟"));
					City[152200] = AreaInfo(150000, 152200, _T("兴安盟"));
					City[640400] = AreaInfo(640000, 640400, _T("固原"));
					City[640200] = AreaInfo(640000, 640200, _T("石嘴山"));
					City[640300] = AreaInfo(640000, 640300, _T("吴忠"));
					City[640100] = AreaInfo(640000, 640100, _T("银川"));
					City[640500] = AreaInfo(640000, 640500, _T("中卫"));
					City[632600] = AreaInfo(630000, 632600, _T("果洛藏族自治州"));
					City[632200] = AreaInfo(630000, 632200, _T("海北藏族自治州"));
					City[632100] = AreaInfo(630000, 632100, _T("海东"));
					City[632500] = AreaInfo(630000, 632500, _T("海南藏族自治州"));
					City[632800] = AreaInfo(630000, 632800, _T("海西蒙古族藏族自治州"));
					City[632300] = AreaInfo(630000, 632300, _T("黄南藏族自治州"));
					City[630100] = AreaInfo(630000, 630100, _T("西宁"));
					City[632700] = AreaInfo(630000, 632700, _T("玉树藏族自治州"));
					City[371600] = AreaInfo(370000, 371600, _T("滨州"));
					City[371400] = AreaInfo(370000, 371400, _T("德州"));
					City[370500] = AreaInfo(370000, 370500, _T("东营"));
					City[371700] = AreaInfo(370000, 371700, _T("菏泽"));
					City[370100] = AreaInfo(370000, 370100, _T("济南"));
					City[370800] = AreaInfo(370000, 370800, _T("济宁"));
					City[371200] = AreaInfo(370000, 371200, _T("莱芜"));
					City[371500] = AreaInfo(370000, 371500, _T("聊城"));
					City[371300] = AreaInfo(370000, 371300, _T("临沂"));
					City[370200] = AreaInfo(370000, 370200, _T("青岛"));
					City[371100] = AreaInfo(370000, 371100, _T("日照"));
					City[370900] = AreaInfo(370000, 370900, _T("泰安"));
					City[371000] = AreaInfo(370000, 371000, _T("威海"));
					City[370700] = AreaInfo(370000, 370700, _T("潍坊"));
					City[370600] = AreaInfo(370000, 370600, _T("烟台"));
					City[370400] = AreaInfo(370000, 370400, _T("枣庄"));
					City[370300] = AreaInfo(370000, 370300, _T("淄博"));
					City[140400] = AreaInfo(140000, 140400, _T("长治"));
					City[140200] = AreaInfo(140000, 140200, _T("大同"));
					City[140500] = AreaInfo(140000, 140500, _T("晋城"));
					City[140700] = AreaInfo(140000, 140700, _T("晋中"));
					City[141000] = AreaInfo(140000, 141000, _T("临汾"));
					City[141100] = AreaInfo(140000, 141100, _T("吕梁"));
					City[140600] = AreaInfo(140000, 140600, _T("朔州"));
					City[140100] = AreaInfo(140000, 140100, _T("太原"));
					City[140900] = AreaInfo(140000, 140900, _T("忻州"));
					City[140300] = AreaInfo(140000, 140300, _T("阳泉"));
					City[140800] = AreaInfo(140000, 140800, _T("运城"));
					City[610900] = AreaInfo(610000, 610900, _T("安康"));
					City[610300] = AreaInfo(610000, 610300, _T("宝鸡"));
					City[610700] = AreaInfo(610000, 610700, _T("汉中"));
					City[611000] = AreaInfo(610000, 611000, _T("商洛"));
					City[610200] = AreaInfo(610000, 610200, _T("铜川"));
					City[610500] = AreaInfo(610000, 610500, _T("渭南"));
					City[610100] = AreaInfo(610000, 610100, _T("西安"));
					City[610400] = AreaInfo(610000, 610400, _T("咸阳"));
					City[610600] = AreaInfo(610000, 610600, _T("延安"));
					City[610800] = AreaInfo(610000, 610800, _T("榆林"));
					City[310101] = AreaInfo(310000, 310101, _T("黄浦"));
					City[310103] = AreaInfo(310000, 310103, _T("卢湾"));
					City[310104] = AreaInfo(310000, 310104, _T("徐汇"));
					City[310105] = AreaInfo(310000, 310105, _T("长宁"));
					City[310106] = AreaInfo(310000, 310106, _T("静安"));
					City[310107] = AreaInfo(310000, 310107, _T("普陀"));
					City[310108] = AreaInfo(110000, 310108, _T("闸北"));
					City[310109] = AreaInfo(310000, 310109, _T("虹口"));
					City[310110] = AreaInfo(310000, 310110, _T("杨浦"));
					City[310112] = AreaInfo(310000, 310112, _T("闵行"));
					City[310113] = AreaInfo(310000, 310113, _T("宝山"));
					City[310114] = AreaInfo(310000, 310114, _T("嘉定"));
					City[310115] = AreaInfo(310000, 310115, _T("浦东新区"));
					City[310116] = AreaInfo(310000, 310116, _T("金山"));
					City[310117] = AreaInfo(310000, 310117, _T("松江"));
					City[310118] = AreaInfo(310000, 310118, _T("青浦"));
					City[310119] = AreaInfo(310000, 310119, _T("南汇"));
					City[310120] = AreaInfo(310000, 310120, _T("奉贤"));
					City[310230] = AreaInfo(310000, 310230, _T("崇明"));




					//----gh
					City[513200] = AreaInfo(510000, 513200, _T("阿坝藏族羌族自治州"));
					City[511900] = AreaInfo(510000, 511900, _T("巴中"));
					City[510100] = AreaInfo(510000, 510100, _T("成都"));
					City[511700] = AreaInfo(510000, 511700, _T("达州"));
					City[510600] = AreaInfo(510000, 510600, _T("德阳"));
					City[513300] = AreaInfo(510000, 513300, _T("甘孜藏族自治州"));
					City[511600] = AreaInfo(510000, 511600, _T("广安"));
					City[510800] = AreaInfo(510000, 510800, _T("广元"));
					City[511100] = AreaInfo(510000, 511100, _T("乐山"));
					City[513400] = AreaInfo(510000, 513400, _T("凉山彝族自治州"));
					City[511400] = AreaInfo(510000, 511400, _T("眉山"));
					City[510700] = AreaInfo(510000, 510700, _T("绵阳"));
					City[511300] = AreaInfo(510000, 511300, _T("南充"));
					City[511000] = AreaInfo(510000, 511000, _T("内江"));
					City[510400] = AreaInfo(510000, 510400, _T("攀枝花"));
					City[510900] = AreaInfo(510000, 510900, _T("遂宁"));
					City[511800] = AreaInfo(510000, 511800, _T("雅安"));
					City[511500] = AreaInfo(510000, 511500, _T("宜宾"));
					City[512000] = AreaInfo(510000, 512000, _T("资阳"));
					City[510300] = AreaInfo(510000, 510300, _T("自贡"));
					City[510500] = AreaInfo(510000, 510500, _T("泸州"));
					City[120101] = AreaInfo(120000, 120101, _T("和平"));
					City[120102] = AreaInfo(120000, 120102, _T("河东"));
					City[120103] = AreaInfo(120000, 120103, _T("河西"));
					City[120104] = AreaInfo(120000, 120104, _T("南开"));
					City[120105] = AreaInfo(120000, 120105, _T("河北"));
					City[120106] = AreaInfo(120000, 120106, _T("红桥"));
					City[120107] = AreaInfo(120000, 120107, _T("塘沽"));
					City[120108] = AreaInfo(120000, 120108, _T("汉沽"));
					City[120109] = AreaInfo(120000, 120109, _T("大港"));
					City[120110] = AreaInfo(120000, 120110, _T("东丽"));
					City[120111] = AreaInfo(120000, 120111, _T("西青"));
					City[120112] = AreaInfo(120000, 120112, _T("津南"));
					City[120113] = AreaInfo(120000, 120113, _T("北辰"));
					City[120114] = AreaInfo(120000, 120114, _T("武清"));
					City[120115] = AreaInfo(120000, 120115, _T("宝坻"));
					City[120221] = AreaInfo(120000, 120221, _T("宁河"));
					City[120223] = AreaInfo(120000, 120223, _T("静海"));
					City[120225] = AreaInfo(120000, 120225, _T("蓟县"));
					City[542500] = AreaInfo(540000, 542500, _T("阿里"));
					City[542100] = AreaInfo(540000, 542100, _T("昌都"));
					City[540100] = AreaInfo(540000, 540100, _T("拉萨"));
					City[542600] = AreaInfo(540000, 542600, _T("林芝"));
					City[542400] = AreaInfo(540000, 542400, _T("那曲"));
					City[542300] = AreaInfo(540000, 542300, _T("日喀则"));
					City[542200] = AreaInfo(540000, 542200, _T("山南"));
					City[652900] = AreaInfo(650000, 652900, _T("阿克苏"));
					City[654200] = AreaInfo(650000, 654200, _T("塔城"));
					City[652800] = AreaInfo(650000, 652800, _T("巴音郭楞蒙古自治州"));
					City[652700] = AreaInfo(650000, 652700, _T("博尔塔拉蒙古自治州"));
					City[652300] = AreaInfo(650000, 652300, _T("昌吉回族自治州"));
					City[652200] = AreaInfo(650000, 652200, _T("哈密"));
					City[653200] = AreaInfo(650000, 653200, _T("和田"));
					City[653100] = AreaInfo(650000, 653100, _T("喀什"));
					City[650200] = AreaInfo(650000, 650200, _T("克拉玛依"));
					City[653000] = AreaInfo(650000, 653000, _T("克孜勒苏柯尔克孜自治州"));
					City[654300] = AreaInfo(650000, 654300, _T("阿勒泰"));
					City[659000] = AreaInfo(650000, 659000, _T("省直辖"));
					City[652100] = AreaInfo(650000, 652100, _T("吐鲁番"));
					City[650100] = AreaInfo(650000, 650100, _T("乌鲁木齐"));
					City[654000] = AreaInfo(650000, 654000, _T("伊犁哈萨克自治州"));
					City[530500] = AreaInfo(530000, 530500, _T("保山"));
					City[532300] = AreaInfo(530000, 532300, _T("楚雄彝族自治州"));
					City[532900] = AreaInfo(530000, 532900, _T("大理白族自治州"));
					City[533100] = AreaInfo(530000, 533100, _T("德宏傣族景颇族自治州"));
					City[533400] = AreaInfo(530000, 533400, _T("迪庆藏族自治州"));
					City[532500] = AreaInfo(530000, 532500, _T("红河哈尼族彝族自治州"));
					City[530100] = AreaInfo(530000, 530100, _T("昆明"));
					City[530700] = AreaInfo(530000, 530700, _T("丽江"));
					City[530900] = AreaInfo(530000, 530900, _T("临沧"));
					City[533300] = AreaInfo(530000, 533300, _T("怒江傈傈族自治州"));
					City[530300] = AreaInfo(530000, 530300, _T("曲靖"));
					City[530800] = AreaInfo(530000, 530800, _T("思茅"));
					City[532600] = AreaInfo(530000, 532600, _T("文山壮族苗族自治州"));
					City[532800] = AreaInfo(530000, 532800, _T("西双版纳傣族自治州"));
					City[530400] = AreaInfo(530000, 530400, _T("玉溪"));
					City[530600] = AreaInfo(530000, 530600, _T("昭通"));
					City[330100] = AreaInfo(330000, 330100, _T("杭州"));
					City[330500] = AreaInfo(330000, 330500, _T("湖州"));
					City[330400] = AreaInfo(330000, 330400, _T("嘉兴"));
					City[330700] = AreaInfo(330000, 330700, _T("金华"));
					City[331100] = AreaInfo(330000, 331100, _T("丽水"));
					City[330200] = AreaInfo(330000, 330200, _T("宁波"));
					City[330600] = AreaInfo(330000, 330600, _T("绍兴"));
					City[331000] = AreaInfo(330000, 331000, _T("台州"));
					City[330300] = AreaInfo(330000, 330300, _T("温州"));
					City[330900] = AreaInfo(330000, 330900, _T("舟山"));
					City[330800] = AreaInfo(330000, 330800, _T("衢州"));
					City[340800] = AreaInfo(340000, 340800, _T("安庆"));
					City[340300] = AreaInfo(340000, 340300, _T("蚌埠"));
					City[341400] = AreaInfo(340000, 341400, _T("巢湖"));
					City[341700] = AreaInfo(340000, 341700, _T("池州"));
					City[341100] = AreaInfo(340000, 341100, _T("滁州"));
					City[341200] = AreaInfo(340000, 341200, _T("阜阳"));
					City[340100] = AreaInfo(340000, 340100, _T("合肥"));
					City[340600] = AreaInfo(340000, 340600, _T("淮北"));
					City[340400] = AreaInfo(340000, 340400, _T("淮南"));
					City[341000] = AreaInfo(340000, 341000, _T("黄山"));
					City[341500] = AreaInfo(340000, 341500, _T("六安"));
					City[340500] = AreaInfo(340000, 340500, _T("马鞍山"));
					City[341300] = AreaInfo(340000, 341300, _T("宿州"));
					City[340700] = AreaInfo(340000, 340700, _T("铜陵"));
					City[340200] = AreaInfo(340000, 340200, _T("芜湖"));
					City[341800] = AreaInfo(340000, 341800, _T("宣城"));
					City[341600] = AreaInfo(340000, 341600, _T("亳州"));
					City[1000001] = AreaInfo(1000000, 1000001, _T("马来西亚"));
					City[1000002] = AreaInfo(1000000, 1000002, _T("泰国"));
					City[1000003] = AreaInfo(1000000, 1000003, _T("印度尼西亚"));
					City[1000004] = AreaInfo(1000000, 1000004, _T("新加坡"));
					City[1000005] = AreaInfo(1000000, 1000005, _T("美国"));
					City[1000006] = AreaInfo(1000000, 1000006, _T("其他"));
					City[2000001] = AreaInfo(2000000, 2000001, _T("北京"));
					City[2000002] = AreaInfo(2000000, 2000002, _T("上海"));
					City[2000003] = AreaInfo(2000000, 2000003, _T("天津"));
					City[2000004] = AreaInfo(2000000, 2000004, _T("重庆"));
					City[2000005] = AreaInfo(2000000, 2000005, _T("福建"));
					City[2000006] = AreaInfo(2000000, 2000006, _T("广东"));
					City[2000007] = AreaInfo(2000000, 2000007, _T("海南"));
					City[2000008] = AreaInfo(2000000, 2000008, _T("河北"));
					City[2000009] = AreaInfo(2000000, 2000009, _T("河南"));
					City[2000010] = AreaInfo(2000000, 2000010, _T("山西"));
					City[2000011] = AreaInfo(2000000, 2000011, _T("云南"));
					City[2000012] = AreaInfo(2000000, 2000012, _T("黑龙江"));
					City[2000013] = AreaInfo(2000000, 2000013, _T("湖南"));
					City[2000014] = AreaInfo(2000000, 2000014, _T("湖北"));
					City[2000015] = AreaInfo(2000000, 2000015, _T("内蒙古"));
					City[2000016] = AreaInfo(2000000, 2000016, _T("浙江"));
					City[2000017] = AreaInfo(2000000, 2000017, _T("江苏"));
					City[2000018] = AreaInfo(2000000, 2000018, _T("江西"));
					City[2000019] = AreaInfo(2000000, 2000019, _T("安徽"));
					City[2000020] = AreaInfo(2000000, 2000020, _T("广西"));
					City[2000021] = AreaInfo(2000000, 2000021, _T("辽宁"));
					City[2000022] = AreaInfo(2000000, 2000022, _T("吉林"));
					City[2000023] = AreaInfo(2000000, 2000023, _T("新疆"));
					City[2000024] = AreaInfo(2000000, 2000024, _T("山东"));
					City[2000025] = AreaInfo(2000000, 2000025, _T("甘肃"));
					City[2000026] = AreaInfo(2000000, 2000026, _T("陕西"));
					City[2000027] = AreaInfo(2000000, 2000027, _T("西藏"));
					City[2000028] = AreaInfo(2000000, 2000028, _T("宁夏"));
					City[2000029] = AreaInfo(2000000, 2000029, _T("青海"));
					City[2000030] = AreaInfo(2000000, 2000030, _T("四川"));
					City[2000031] = AreaInfo(2000000, 2000031, _T("贵州"));





					//---yp

					City[110101] = AreaInfo(110000, 110101, _T("东城"));
					City[110102] = AreaInfo(110000, 110102, _T("西城"));
					City[110103] = AreaInfo(110000, 110103, _T("崇文"));
					City[110104] = AreaInfo(110000, 110104, _T("宣武"));
					City[110105] = AreaInfo(110000, 110105, _T("朝阳"));
					City[110106] = AreaInfo(110000, 110106, _T("丰台"));
					City[110107] = AreaInfo(110000, 110107, _T("石景山"));
					City[110108] = AreaInfo(110000, 110108, _T("海淀"));
					City[110109] = AreaInfo(110000, 110109, _T("门头沟"));
					City[110111] = AreaInfo(110000, 110111, _T("房山"));
					City[110112] = AreaInfo(110000, 110112, _T("通州"));
					City[110113] = AreaInfo(110000, 110113, _T("顺义"));
					City[110114] = AreaInfo(110000, 110114, _T("昌平"));
					City[110115] = AreaInfo(110000, 110115, _T("大兴"));
					City[110116] = AreaInfo(110000, 110116, _T("怀柔"));
					City[110117] = AreaInfo(110000, 110117, _T("平谷"));
					City[110228] = AreaInfo(110000, 110228, _T("密云"));
					City[110229] = AreaInfo(110000, 110229, _T("延庆"));

					City[500101] = AreaInfo(500000, 500101, _T("万州"));
					City[500102] = AreaInfo(500000, 500102, _T("涪陵"));
					City[500103] = AreaInfo(500000, 500103, _T("渝中区"));
					City[500104] = AreaInfo(500000, 500104, _T("大渡口"));
					City[500105] = AreaInfo(500000, 500105, _T("江北"));
					City[500106] = AreaInfo(500000, 500106, _T("沙坪坝"));
					City[500107] = AreaInfo(500000, 500107, _T("九龙坡"));
					City[500108] = AreaInfo(500000, 500108, _T("南岸"));
					City[500109] = AreaInfo(500000, 500109, _T("北碚"));
					City[500110] = AreaInfo(500000, 500110, _T("万盛"));
					City[500111] = AreaInfo(500000, 500111, _T("双桥"));
					City[500112] = AreaInfo(500000, 500112, _T("渝北"));
					City[500113] = AreaInfo(500000, 500113, _T("巴南"));
					City[500114] = AreaInfo(500000, 500114, _T("黔江"));
					City[500115] = AreaInfo(500000, 500115, _T("长寿"));
					City[500222] = AreaInfo(500000, 500222, _T("綦江"));
					City[500223] = AreaInfo(500000, 500223, _T("潼南"));
					City[500224] = AreaInfo(500000, 500224, _T("铜梁"));
					City[500225] = AreaInfo(500000, 500225, _T("大足"));
					City[500226] = AreaInfo(500000, 500226, _T("荣昌"));
					City[500227] = AreaInfo(500000, 500227, _T("璧山"));
					City[500228] = AreaInfo(500000, 500228, _T("梁平"));
					City[500229] = AreaInfo(500000, 500229, _T("城口"));
					City[500230] = AreaInfo(500000, 500230, _T("丰都"));
					City[500231] = AreaInfo(500000, 500231, _T("垫江"));
					City[500232] = AreaInfo(500000, 500232, _T("武隆"));
					City[500233] = AreaInfo(500000, 500233, _T("忠县"));
					City[500234] = AreaInfo(500000, 500234, _T("开县"));
					City[500235] = AreaInfo(500000, 500235, _T("云阳"));
					City[500236] = AreaInfo(500000, 500236, _T("奉节"));
					City[500237] = AreaInfo(500000, 500237, _T("巫山"));
					City[500238] = AreaInfo(500000, 500238, _T("巫溪"));
					City[500240] = AreaInfo(500000, 500240, _T("石柱"));
					City[500241] = AreaInfo(500000, 500241, _T("秀山"));
					City[500242] = AreaInfo(500000, 500242, _T("酉阳"));
					City[500243] = AreaInfo(500000, 500243, _T("彭水"));
					City[500381] = AreaInfo(500000, 500381, _T("江津"));
					City[500382] = AreaInfo(500000, 500382, _T("合川"));
					City[500383] = AreaInfo(500000, 500383, _T("永川"));
					City[500384] = AreaInfo(500000, 500384, _T("南川"));

					City[350100] = AreaInfo(350000, 350100, _T("福州"));
					City[350800] = AreaInfo(350000, 350800, _T("龙岩"));
					City[350700] = AreaInfo(350000, 350700, _T("南平"));
					City[350900] = AreaInfo(350000, 350900, _T("宁德"));
					City[350300] = AreaInfo(350000, 350300, _T("莆田"));
					City[350500] = AreaInfo(350000, 350500, _T("泉州"));
					City[350400] = AreaInfo(350000, 350400, _T("三明"));
					City[350200] = AreaInfo(350000, 350200, _T("厦门"));
					City[350600] = AreaInfo(350000, 350600, _T("漳州"));

					City[620400] = AreaInfo(620000, 620400, _T("白银"));
					City[621100] = AreaInfo(620000, 621100, _T("定西"));
					City[623000] = AreaInfo(620000, 623000, _T("甘南藏族自治州"));
					City[620200] = AreaInfo(620000, 620200, _T("嘉峪关"));
					City[620300] = AreaInfo(620000, 620300, _T("金昌"));
					City[620900] = AreaInfo(620000, 620900, _T("酒泉"));
					City[620100] = AreaInfo(620000, 620100, _T("兰州"));
					City[622900] = AreaInfo(620000, 622900, _T("临夏回族自治州"));
					City[621200] = AreaInfo(620000, 621200, _T("陇南"));
					City[620800] = AreaInfo(620000, 620800, _T("平凉"));
					City[621000] = AreaInfo(620000, 621000, _T("庆阳"));
					City[620500] = AreaInfo(620000, 620500, _T("天水"));
					City[620600] = AreaInfo(620000, 620600, _T("武威"));
					City[620700] = AreaInfo(620000, 620700, _T("张掖"));

					City[445100] = AreaInfo(440000, 445100, _T("潮州"));
					City[441900] = AreaInfo(440000, 441900, _T("东莞"));
					City[440600] = AreaInfo(440000, 440600, _T("佛山"));
					City[440100] = AreaInfo(440000, 440100, _T("广州"));
					City[441600] = AreaInfo(440000, 441600, _T("河源"));
					City[441300] = AreaInfo(440000, 441300, _T("惠州"));
					City[440700] = AreaInfo(440000, 440700, _T("江门"));
					City[445200] = AreaInfo(440000, 445200, _T("揭阳"));
					City[440900] = AreaInfo(440000, 440900, _T("茂名"));
					City[441400] = AreaInfo(440000, 441400, _T("梅州"));
					City[441800] = AreaInfo(440000, 441800, _T("清远"));
					City[440500] = AreaInfo(440000, 440500, _T("汕头"));
					City[441500] = AreaInfo(440000, 441500, _T("汕尾"));
					City[440200] = AreaInfo(440000, 440200, _T("韶关"));
					City[440300] = AreaInfo(440000, 440300, _T("深圳"));
					City[441700] = AreaInfo(440000, 441700, _T("阳江"));
					City[445300] = AreaInfo(440000, 445300, _T("云浮"));
					City[440800] = AreaInfo(440000, 440800, _T("湛江"));
					City[441200] = AreaInfo(440000, 441200, _T("肇庆"));
					City[442000] = AreaInfo(440000, 442000, _T("中山"));
					City[440400] = AreaInfo(440000, 440400, _T("珠海"));

					City[451000] = AreaInfo(450000, 451000, _T("百色"));
					City[450500] = AreaInfo(450000, 450500, _T("北海"));
					City[451400] = AreaInfo(450000, 451400, _T("崇左"));
					City[450600] = AreaInfo(450000, 450600, _T("防城港"));
					City[450300] = AreaInfo(450000, 450300, _T("桂林"));
					City[450800] = AreaInfo(450000, 450800, _T("贵港"));
					City[451200] = AreaInfo(450000, 451200, _T("河池"));
					City[451100] = AreaInfo(450000, 451100, _T("贺州"));
					City[451300] = AreaInfo(450000, 451300, _T("来宾"));
					City[450200] = AreaInfo(450000, 450200, _T("柳州"));
					City[450100] = AreaInfo(450000, 450100, _T("南宁"));
					City[450700] = AreaInfo(450000, 450700, _T("钦州"));
					City[450400] = AreaInfo(450000, 450400, _T("梧州"));
					City[450900] = AreaInfo(450000, 450900, _T("玉林"));

					City[520400] = AreaInfo(520000, 520400, _T("安顺"));
					City[522400] = AreaInfo(520000, 522400, _T("毕节"));
					City[520100] = AreaInfo(520000, 520100, _T("贵阳"));
					City[520200] = AreaInfo(520000, 520200, _T("六盘水"));
					City[522600] = AreaInfo(520000, 522600, _T("黔东南苗族侗族自治州"));
					City[522700] = AreaInfo(520000, 522700, _T("黔南布依族苗族自治州"));
					City[522300] = AreaInfo(520000, 522300, _T("黔西南布依族苗族自治州"));
					City[522200] = AreaInfo(520000, 522200, _T("铜仁"));
					City[520300] = AreaInfo(520000, 520300, _T("遵义"));

					City[469000] = AreaInfo(460000, 469000, _T("省直辖"));
					City[460100] = AreaInfo(460000, 460100, _T("海口"));
					City[460200] = AreaInfo(460000, 460200, _T("三亚"));

					City[130600] = AreaInfo(130000, 130600, _T("保定"));
					City[130900] = AreaInfo(130000, 130900, _T("沧州"));
					City[130800] = AreaInfo(130000, 130800, _T("承德"));
					City[130400] = AreaInfo(130000, 130400, _T("邯郸"));
					City[131100] = AreaInfo(130000, 131100, _T("衡水"));
					City[131000] = AreaInfo(130000, 131000, _T("廊坊"));
					City[130300] = AreaInfo(130000, 130300, _T("秦皇岛"));
					City[130100] = AreaInfo(130000, 130100, _T("石家庄"));
					City[130200] = AreaInfo(130000, 130200, _T("唐山"));
					City[130500] = AreaInfo(130000, 130500, _T("邢台"));
					City[130700] = AreaInfo(130000, 130700, _T("张家口"));

					City[410500] = AreaInfo(410000, 410500, _T("安阳"));
					City[410600] = AreaInfo(410000, 410600, _T("鹤壁"));
					City[410800] = AreaInfo(410000, 410800, _T("焦作"));
					City[410200] = AreaInfo(410000, 410200, _T("开封"));
					City[410300] = AreaInfo(410000, 410300, _T("洛阳"));
					City[411300] = AreaInfo(410000, 411300, _T("南阳"));
					City[410400] = AreaInfo(410000, 410400, _T("平顶山"));
					City[411200] = AreaInfo(410000, 411200, _T("三门峡"));
					City[411400] = AreaInfo(410000, 411400, _T("商丘"));
					City[410700] = AreaInfo(410000, 410700, _T("新乡"));
					City[411500] = AreaInfo(410000, 411500, _T("信阳"));
					City[411000] = AreaInfo(410000, 411000, _T("许昌"));
					City[410100] = AreaInfo(410000, 410100, _T("郑州"));
					City[411600] = AreaInfo(410000, 411600, _T("周口"));
					City[411700] = AreaInfo(410000, 411700, _T("驻马店"));
					City[411100] = AreaInfo(410000, 411100, _T("漯河"));
					City[410900] = AreaInfo(410000, 410900, _T("濮阳"));
					City[411800] = AreaInfo(410000, 411800, _T("济源"));
				}
			}initArea;
			

			void GetProvinceName(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				for(ProvinceInfo::const_iterator iter = Provience.begin(); iter != Provience.end(); ++iter)
				{
					callback(iter->second.Name, iter->first);
				}
			}

			void GetCityName(size_t parentID, const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				for(CityInfo::const_iterator iter = City.begin(); iter != City.end(); ++iter)
				{
					if( parentID == iter->second.ParentID )
						callback(iter->second.Name, iter->first);
				}
			}


			namespace detail
			{
				static struct
				{
					int type_;
					LPCTSTR name_;
				}diskType[] = 
				{
					{ 1, _T("无盘") },
					{ 2, _T("有盘") },
					{ 3, _T("混合") }
				};


				static struct
				{
					int type_;
					LPCTSTR name_;
				}networkType[] = 
				{
					{ 1, _T("其它") },
					{ 2, _T("ADSL") },
					{ 3, _T("光纤") }
				};
			}

			void GetDiskType(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::diskType); ++i)
					callback(detail::diskType[i].name_, detail::diskType[i].type_);
			}

			void GetNetworkType(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				for(size_t i = 0; i != _countof(detail::networkType); ++i)
					callback(detail::networkType[i].name_, detail::networkType[i].type_);
			}

			stdex::tString GetDiskNameByType(size_t type)
			{
				for(size_t i = 0; i != _countof(detail::diskType); ++i)
				{
					if( type == detail::diskType[i].type_ )
						return detail::diskType[i].name_;
				}

				return _T("");
			}

			stdex::tString GetNetworkByType(size_t type)
			{
				for(size_t i = 0; i != _countof(detail::networkType); ++i)
				{
					if( type == detail::networkType[i].type_ )
						return detail::networkType[i].name_;
				}

				return _T("");
			}

			// 获取无盘产品名称
			void GetDiskProduct(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				static struct
				{
					LPCTSTR type_;
					LPCTSTR name_;
				}diskProduct[] = 
				{
					{ _T("无"),		_T("无") },
					{ _T("其他"),	_T("其他") },
					{ _T("锐起"),	_T("锐起") },
					{ _T("网众无盘"),	_T("网众无盘") },
					{ _T("顺网无盘"),	_T("顺网无盘") },
					{ _T("易游"),	_T("易游") },
					{ _T("DOL"),	_T("DOL") },
					{ _T("VHD"),	_T("VHD") },
					{ _T("信佑无盘"),	_T("信佑无盘") },
					{ _T("快吧无盘"),	_T("快吧无盘") },
					{ _T("MZD"),	_T("MZD") },
					{ _T("奇东锐腾"),	_T("奇东锐腾") }
				};

				for(size_t i = 0; i != _countof(diskProduct); ++i)
					callback(diskProduct[i].name_, (int)diskProduct[i].type_);
			}


			// 获取收费软件名称
			void GetFeeProductName(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				static struct
				{
					LPCTSTR type_;
					LPCTSTR name_;
				}feeProduct[] = 
				{
					{ _T("无"),		_T("无") },
					{ _T("其他"),	_T("其他") },
					{ _T("吉胜万象"),	_T("吉胜万象") },
					{ _T("新浩艺Pubwin"),	_T("新浩艺Pubwin") },
					{ _T("雨人一卡通"),	_T("雨人一卡通") },
					{ _T("嘟嘟牛"),	_T("嘟嘟牛") },
					{ _T("龙管家"),	_T("龙管家") },
					{ _T("恒信一卡通"),	_T("恒信一卡通") },
					{ _T("长春同联"),	_T("长春同联") },
					{ _T("美萍"),	_T("美萍") }
				};

				for(size_t i = 0; i != _countof(feeProduct); ++i)
					callback(feeProduct[i].name_, (int)feeProduct[i].type_);
			}

			// 获取文化软件名称
			void GetCulturalProductName(const std::tr1::function<void(LPCTSTR, size_t)> &callback)
			{
				static struct
				{
					LPCTSTR type_;
					LPCTSTR name_;
				}culturalProduct[] = 
				{
					{ _T("无"),		_T("无") },
					{ _T("其他"),	_T("其他") },
					{ _T("净网先锋"),	_T("净网先锋") },
					{ _T("任子行"),	_T("任子行") },
					{ _T("竞天微注"),	_T("竞天微注") },
					{ _T("矮哨兵"),	_T("矮哨兵") }
				};

				for(size_t i = 0; i != _countof(culturalProduct); ++i)
					callback(culturalProduct[i].name_, (int)culturalProduct[i].type_);
			}

			bool IsValidPath(LPCTSTR path)
			{
				stdex::tString Dvr;
				Dvr = path;
				size_t pos = Dvr.find_first_of(_T('\\'));
				Dvr = Dvr.substr(0, pos);
				if(!PathFileExists(Dvr.c_str()))
					return false;

				TCHAR text[MAX_PATH] = {0};
				utility::Strcpy(text, path);
				::PathRemoveBackslash(text);

				if(!PathFileExists((LPCTSTR)text))
				{
					if( !utility::mkpath((LPCTSTR)text) )
						return false;
				}

				return true;

			}

		}
	}
}