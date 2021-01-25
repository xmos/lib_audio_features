#include "xs3_fft_lut.h"

const complex_s32_t xs3_dit_fft_lut[508] = 
	{
	{ 1073741824,           0}, {  759250125,  -759250125}, {          0, -1073741824}, { -759250125,  -759250125}, 
	{          0, -1073741824}, { -410903207,  -992008094}, { -759250125,  -759250125}, { -992008094,  -410903207}, 
	{ 1073741824,           0}, {  992008094,  -410903207}, {  759250125,  -759250125}, {  410903207,  -992008094}, 
	{ -759250125,  -759250125}, { -892783698,  -596538995}, { -992008094,  -410903207}, {-1053110176,  -209476638}, 
	{          0, -1073741824}, { -209476638, -1053110176}, { -410903207,  -992008094}, { -596538995,  -892783698}, 
	{  759250125,  -759250125}, {  596538995,  -892783698}, {  410903207,  -992008094}, {  209476638, -1053110176}, 
	{ 1073741824,           0}, { 1053110176,  -209476638}, {  992008094,  -410903207}, {  892783698,  -596538995}, 
	{ -992008094,  -410903207}, {-1027506862,  -311690799}, {-1053110176,  -209476638}, {-1068571464,  -105245103}, 
	{ -759250125,  -759250125}, { -830013654,  -681174602}, { -892783698,  -596538995}, { -946955747,  -506158392}, 
	{ -410903207,  -992008094}, { -506158392,  -946955747}, { -596538995,  -892783698}, { -681174602,  -830013654}, 
	{          0, -1073741824}, { -105245103, -1068571464}, { -209476638, -1053110176}, { -311690799, -1027506862}, 
	{  410903207,  -992008094}, {  311690799, -1027506862}, {  209476638, -1053110176}, {  105245103, -1068571464}, 
	{  759250125,  -759250125}, {  681174602,  -830013654}, {  596538995,  -892783698}, {  506158392,  -946955747}, 
	{  992008094,  -410903207}, {  946955747,  -506158392}, {  892783698,  -596538995}, {  830013654,  -681174602}, 
	{ 1073741824,           0}, { 1068571464,  -105245103}, { 1053110176,  -209476638}, { 1027506862,  -311690799}, 
	{-1053110176,  -209476638}, {-1062120190,  -157550647}, {-1068571464,  -105245103}, {-1072448455,   -52686014}, 
	{ -992008094,  -410903207}, {-1010975242,  -361732726}, {-1027506862,  -311690799}, {-1041563127,  -260897982}, 
	{ -892783698,  -596538995}, { -920979082,  -552013618}, { -946955747,  -506158392}, { -970651112,  -459083786}, 
	{ -759250125,  -759250125}, { -795590213,  -721080937}, { -830013654,  -681174602}, { -862437520,  -639627258}, 
	{ -596538995,  -892783698}, { -639627258,  -862437520}, { -681174602,  -830013654}, { -721080937,  -795590213}, 
	{ -410903207,  -992008094}, { -459083786,  -970651112}, { -506158392,  -946955747}, { -552013618,  -920979082}, 
	{ -209476638, -1053110176}, { -260897982, -1041563127}, { -311690799, -1027506862}, { -361732726, -1010975242}, 
	{          0, -1073741824}, {  -52686014, -1072448455}, { -105245103, -1068571464}, { -157550647, -1062120190}, 
	{  209476638, -1053110176}, {  157550647, -1062120190}, {  105245103, -1068571464}, {   52686014, -1072448455}, 
	{  410903207,  -992008094}, {  361732726, -1010975242}, {  311690799, -1027506862}, {  260897982, -1041563127}, 
	{  596538995,  -892783698}, {  552013618,  -920979082}, {  506158392,  -946955747}, {  459083786,  -970651112}, 
	{  759250125,  -759250125}, {  721080937,  -795590213}, {  681174602,  -830013654}, {  639627258,  -862437520}, 
	{  892783698,  -596538995}, {  862437520,  -639627258}, {  830013654,  -681174602}, {  795590213,  -721080937}, 
	{  992008094,  -410903207}, {  970651112,  -459083786}, {  946955747,  -506158392}, {  920979082,  -552013618}, 
	{ 1053110176,  -209476638}, { 1041563127,  -260897982}, { 1027506862,  -311690799}, { 1010975242,  -361732726}, 
	{ 1073741824,           0}, { 1072448455,   -52686014}, { 1068571464,  -105245103}, { 1062120190,  -157550647}, 
	{-1068571464,  -105245103}, {-1070832474,   -78989349}, {-1072448455,   -52686014}, {-1073418433,   -26350943}, 
	{-1053110176,  -209476638}, {-1057933813,  -183568930}, {-1062120190,  -157550647}, {-1065666786,  -131437462}, 
	{-1027506862,  -311690799}, {-1034846671,  -286380643}, {-1041563127,  -260897982}, {-1047652185,  -235258165}, 
	{ -992008094,  -410903207}, {-1001793390,  -386434353}, {-1010975242,  -361732726}, {-1019548121,  -336813204}, 
	{ -946955747,  -506158392}, { -959092290,  -482766489}, { -970651112,  -459083786}, { -981625251,  -435124548}, 
	{ -892783698,  -596538995}, { -907154608,  -574449320}, { -920979082,  -552013618}, { -934248793,  -529245404}, 
	{ -830013654,  -681174602}, { -846480531,  -660599890}, { -862437520,  -639627258}, { -877875009,  -618269338}, 
	{ -759250125,  -759250125}, { -777654384,  -740388522}, { -795590213,  -721080937}, { -813046808,  -701339000}, 
	{ -681174602,  -830013654}, { -701339000,  -813046808}, { -721080937,  -795590213}, { -740388522,  -777654384}, 
	{ -596538995,  -892783698}, { -618269338,  -877875009}, { -639627258,  -862437520}, { -660599890,  -846480531}, 
	{ -506158392,  -946955747}, { -529245404,  -934248793}, { -552013618,  -920979082}, { -574449320,  -907154608}, 
	{ -410903207,  -992008094}, { -435124548,  -981625251}, { -459083786,  -970651112}, { -482766489,  -959092290}, 
	{ -311690799, -1027506862}, { -336813204, -1019548121}, { -361732726, -1010975242}, { -386434353, -1001793390}, 
	{ -209476638, -1053110176}, { -235258165, -1047652185}, { -260897982, -1041563127}, { -286380643, -1034846671}, 
	{ -105245103, -1068571464}, { -131437462, -1065666786}, { -157550647, -1062120190}, { -183568930, -1057933813}, 
	{          0, -1073741824}, {  -26350943, -1073418433}, {  -52686014, -1072448455}, {  -78989349, -1070832474}, 
	{  105245103, -1068571464}, {   78989349, -1070832474}, {   52686014, -1072448455}, {   26350943, -1073418433}, 
	{  209476638, -1053110176}, {  183568930, -1057933813}, {  157550647, -1062120190}, {  131437462, -1065666786}, 
	{  311690799, -1027506862}, {  286380643, -1034846671}, {  260897982, -1041563127}, {  235258165, -1047652185}, 
	{  410903207,  -992008094}, {  386434353, -1001793390}, {  361732726, -1010975242}, {  336813204, -1019548121}, 
	{  506158392,  -946955747}, {  482766489,  -959092290}, {  459083786,  -970651112}, {  435124548,  -981625251}, 
	{  596538995,  -892783698}, {  574449320,  -907154608}, {  552013618,  -920979082}, {  529245404,  -934248793}, 
	{  681174602,  -830013654}, {  660599890,  -846480531}, {  639627258,  -862437520}, {  618269338,  -877875009}, 
	{  759250125,  -759250125}, {  740388522,  -777654384}, {  721080937,  -795590213}, {  701339000,  -813046808}, 
	{  830013654,  -681174602}, {  813046808,  -701339000}, {  795590213,  -721080937}, {  777654384,  -740388522}, 
	{  892783698,  -596538995}, {  877875009,  -618269338}, {  862437520,  -639627258}, {  846480531,  -660599890}, 
	{  946955747,  -506158392}, {  934248793,  -529245404}, {  920979082,  -552013618}, {  907154608,  -574449320}, 
	{  992008094,  -410903207}, {  981625251,  -435124548}, {  970651112,  -459083786}, {  959092290,  -482766489}, 
	{ 1027506862,  -311690799}, { 1019548121,  -336813204}, { 1010975242,  -361732726}, { 1001793390,  -386434353}, 
	{ 1053110176,  -209476638}, { 1047652185,  -235258165}, { 1041563127,  -260897982}, { 1034846671,  -286380643}, 
	{ 1068571464,  -105245103}, { 1065666786,  -131437462}, { 1062120190,  -157550647}, { 1057933813,  -183568930}, 
	{ 1073741824,           0}, { 1073418433,   -26350943}, { 1072448455,   -52686014}, { 1070832474,   -78989349}, 
	{-1072448455,   -52686014}, {-1073014240,   -39521455}, {-1073418433,   -26350943}, {-1073660973,   -13176464}, 
	{-1068571464,  -105245103}, {-1069782521,   -92124163}, {-1070832474,   -78989349}, {-1071721163,   -65842639}, 
	{-1062120190,  -157550647}, {-1063973603,  -144504935}, {-1065666786,  -131437462}, {-1067199483,  -118350194}, 
	{-1053110176,  -209476638}, {-1055601479,  -196537583}, {-1057933813,  -183568930}, {-1060106826,  -170572633}, 
	{-1041563127,  -260897982}, {-1044686319,  -248096755}, {-1047652185,  -235258165}, {-1050460278,  -222384147}, 
	{-1027506862,  -311690799}, {-1031254418,  -299058239}, {-1034846671,  -286380643}, {-1038283080,  -273659918}, 
	{-1010975242,  -361732726}, {-1015338134,  -349299266}, {-1019548121,  -336813204}, {-1023604567,  -324276419}, 
	{ -992008094,  -410903207}, { -996975812,  -398698801}, {-1001793390,  -386434353}, {-1006460100,  -374111709}, 
	{ -970651112,  -459083786}, { -976211688,  -447137835}, { -981625251,  -435124548}, { -986890984,  -423045732}, 
	{ -946955747,  -506158392}, { -953095785,  -494499676}, { -959092290,  -482766489}, { -964944360,  -470960600}, 
	{ -920979082,  -552013618}, { -927683790,  -540670223}, { -934248793,  -529245404}, { -940673101,  -517740883}, 
	{ -892783698,  -596538995}, { -900036924,  -585538248}, { -907154608,  -574449320}, { -914135678,  -563273883}, 
	{ -862437520,  -639627258}, { -870221790,  -628995660}, { -877875009,  -618269338}, { -885396022,  -607449906}, 
	{ -830013654,  -681174602}, { -838310216,  -670937767}, { -846480531,  -660599890}, { -854523370,  -650162530}, 
	{ -795590213,  -721080937}, { -804379079,  -711263525}, { -813046808,  -701339000}, { -821592095,  -691308855}, 
	{ -759250125,  -759250125}, { -768510122,  -749875788}, { -777654384,  -740388522}, { -786681534,  -730789757}, 
	{ -721080937,  -795590213}, { -730789757,  -786681534}, { -740388522,  -777654384}, { -749875788,  -768510122}, 
	{ -681174602,  -830013654}, { -691308855,  -821592095}, { -701339000,  -813046808}, { -711263525,  -804379079}, 
	{ -639627258,  -862437520}, { -650162530,  -854523370}, { -660599890,  -846480531}, { -670937767,  -838310216}, 
	{ -596538995,  -892783698}, { -607449906,  -885396022}, { -618269338,  -877875009}, { -628995660,  -870221790}, 
	{ -552013618,  -920979082}, { -563273883,  -914135678}, { -574449320,  -907154608}, { -585538248,  -900036924}, 
	{ -506158392,  -946955747}, { -517740883,  -940673101}, { -529245404,  -934248793}, { -540670223,  -927683790}, 
	{ -459083786,  -970651112}, { -470960600,  -964944360}, { -482766489,  -959092290}, { -494499676,  -953095785}, 
	{ -410903207,  -992008094}, { -423045732,  -986890984}, { -435124548,  -981625251}, { -447137835,  -976211688}, 
	{ -361732726, -1010975242}, { -374111709, -1006460100}, { -386434353, -1001793390}, { -398698801,  -996975812}, 
	{ -311690799, -1027506862}, { -324276419, -1023604567}, { -336813204, -1019548121}, { -349299266, -1015338134}, 
	{ -260897982, -1041563127}, { -273659918, -1038283080}, { -286380643, -1034846671}, { -299058239, -1031254418}, 
	{ -209476638, -1053110176}, { -222384147, -1050460278}, { -235258165, -1047652185}, { -248096755, -1044686319}, 
	{ -157550647, -1062120190}, { -170572633, -1060106826}, { -183568930, -1057933813}, { -196537583, -1055601479}, 
	{ -105245103, -1068571464}, { -118350194, -1067199483}, { -131437462, -1065666786}, { -144504935, -1063973603}, 
	{  -52686014, -1072448455}, {  -65842639, -1071721163}, {  -78989349, -1070832474}, {  -92124163, -1069782521}, 
	{          0, -1073741824}, {  -13176464, -1073660973}, {  -26350943, -1073418433}, {  -39521455, -1073014240}, 
	{   52686014, -1072448455}, {   39521455, -1073014240}, {   26350943, -1073418433}, {   13176464, -1073660973}, 
	{  105245103, -1068571464}, {   92124163, -1069782521}, {   78989349, -1070832474}, {   65842639, -1071721163}, 
	{  157550647, -1062120190}, {  144504935, -1063973603}, {  131437462, -1065666786}, {  118350194, -1067199483}, 
	{  209476638, -1053110176}, {  196537583, -1055601479}, {  183568930, -1057933813}, {  170572633, -1060106826}, 
	{  260897982, -1041563127}, {  248096755, -1044686319}, {  235258165, -1047652185}, {  222384147, -1050460278}, 
	{  311690799, -1027506862}, {  299058239, -1031254418}, {  286380643, -1034846671}, {  273659918, -1038283080}, 
	{  361732726, -1010975242}, {  349299266, -1015338134}, {  336813204, -1019548121}, {  324276419, -1023604567}, 
	{  410903207,  -992008094}, {  398698801,  -996975812}, {  386434353, -1001793390}, {  374111709, -1006460100}, 
	{  459083786,  -970651112}, {  447137835,  -976211688}, {  435124548,  -981625251}, {  423045732,  -986890984}, 
	{  506158392,  -946955747}, {  494499676,  -953095785}, {  482766489,  -959092290}, {  470960600,  -964944360}, 
	{  552013618,  -920979082}, {  540670223,  -927683790}, {  529245404,  -934248793}, {  517740883,  -940673101}, 
	{  596538995,  -892783698}, {  585538248,  -900036924}, {  574449320,  -907154608}, {  563273883,  -914135678}, 
	{  639627258,  -862437520}, {  628995660,  -870221790}, {  618269338,  -877875009}, {  607449906,  -885396022}, 
	{  681174602,  -830013654}, {  670937767,  -838310216}, {  660599890,  -846480531}, {  650162530,  -854523370}, 
	{  721080937,  -795590213}, {  711263525,  -804379079}, {  701339000,  -813046808}, {  691308855,  -821592095}, 
	{  759250125,  -759250125}, {  749875788,  -768510122}, {  740388522,  -777654384}, {  730789757,  -786681534}, 
	{  795590213,  -721080937}, {  786681534,  -730789757}, {  777654384,  -740388522}, {  768510122,  -749875788}, 
	{  830013654,  -681174602}, {  821592095,  -691308855}, {  813046808,  -701339000}, {  804379079,  -711263525}, 
	{  862437520,  -639627258}, {  854523370,  -650162530}, {  846480531,  -660599890}, {  838310216,  -670937767}, 
	{  892783698,  -596538995}, {  885396022,  -607449906}, {  877875009,  -618269338}, {  870221790,  -628995660}, 
	{  920979082,  -552013618}, {  914135678,  -563273883}, {  907154608,  -574449320}, {  900036924,  -585538248}, 
	{  946955747,  -506158392}, {  940673101,  -517740883}, {  934248793,  -529245404}, {  927683790,  -540670223}, 
	{  970651112,  -459083786}, {  964944360,  -470960600}, {  959092290,  -482766489}, {  953095785,  -494499676}, 
	{  992008094,  -410903207}, {  986890984,  -423045732}, {  981625251,  -435124548}, {  976211688,  -447137835}, 
	{ 1010975242,  -361732726}, { 1006460100,  -374111709}, { 1001793390,  -386434353}, {  996975812,  -398698801}, 
	{ 1027506862,  -311690799}, { 1023604567,  -324276419}, { 1019548121,  -336813204}, { 1015338134,  -349299266}, 
	{ 1041563127,  -260897982}, { 1038283080,  -273659918}, { 1034846671,  -286380643}, { 1031254418,  -299058239}, 
	{ 1053110176,  -209476638}, { 1050460278,  -222384147}, { 1047652185,  -235258165}, { 1044686319,  -248096755}, 
	{ 1062120190,  -157550647}, { 1060106826,  -170572633}, { 1057933813,  -183568930}, { 1055601479,  -196537583}, 
	{ 1068571464,  -105245103}, { 1067199483,  -118350194}, { 1065666786,  -131437462}, { 1063973603,  -144504935}, 
	{ 1072448455,   -52686014}, { 1071721163,   -65842639}, { 1070832474,   -78989349}, { 1069782521,   -92124163}, 
	{ 1073741824,           0}, { 1073660973,   -13176464}, { 1073418433,   -26350943}, { 1073014240,   -39521455}, 
	};
