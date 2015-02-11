#include "include/plate_recognize.h"
#include "include/util.h"
#include "include/features.h"
#include "include/CParser.h"

using namespace easypr;

int svmMain();
int acurayTestMain();
void cmdMain(int argc, char *argv[]);

namespace easypr {

	int svmTrain(bool dividePrepared = true, bool trainPrepared = true,
	svmCallback getFeatures = getHistogramFeatures);

}

extern const string GENERAL_TEST_PATH = "image/general_test";
extern const string NATIVE_TEST_PATH = "image/native_test";

////////////////////////////////////////////////////////////
// EasyPR ËÆ≠ÁªÉÂëΩ‰ª§Ë°å

const string option[] = 
	{
		"1. ÊµãËØï;"	,
		"2. ÊâπÈáèÊµãËØï;"		,
		"3. SVMËÆ≠ÁªÉ;"		,
		"4. ANNËÆ≠ÁªÉ(Êú™ÂºÄÊîæ);"		,
		"5. GDTSÁîüÊàê;"		,
		"6. ÂºÄÂèëÂõ¢Èòü;"		,
		"7. ÊÑüË∞¢ÂêçÂçï;"		,
		"8. ÈÄÄÂá∫;"			,  
	};

const int optionCount = 8;

int main(int argc, char *argv[])
{
    if (argc > 1) {
        //enable command line option
        cmdMain(argc, argv);
        return 0;
    }

	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "EasyPR Option:"<< endl;
		for(int i = 0; i < optionCount; i++)
		{
			selectOption << option[i] << endl;
		}

		cout << "////////////////////////////////////"<< endl;
		cout << selectOption.str();
		cout << "////////////////////////////////////"<< endl;
		cout << "ËØ∑ÈÄâÊã©‰∏ÄÈ°πÊìç‰Ωú:";

		int select = -1;
		bool isRepeat = true;
		while (isRepeat)
		{
			cin >> select;
			isRepeat = false;
			switch (select)
			{
			case 1:
				testMain();
				break;
			case 2:
				acurayTestMain();
				break;
			case 3:
				svmMain();
				break;
			case 4:
				// TODO
				break;
			case 5:				
				//general_test();
				generate_gdts();
				break;
			case 6:
				// ÂºÄÂèëÂõ¢Èòü;
				cout << endl;
				cout << "Êàë‰ª¨EasyPRÂõ¢ÈòüÁõÆÂâçÊúâ‰∏Ä‰∏™5‰∫∫Â∑¶Âè≥ÁöÑÂ∞èÁªÑÂú®ËøõË°åEasyPRÂêéÁª≠ÁâàÊú¨ÁöÑÂºÄÂèëÂ∑•‰Ωú„ÄÇ" << endl;
				cout << "Â¶ÇÊûú‰Ω†ÂØπÊú¨È°πÁõÆÊÑüÂÖ¥Ë∂£ÔºåÂπ∂‰∏îÊÑøÊÑè‰∏∫ÂºÄÊ∫êË¥°ÁåÆ‰∏Ä‰ªΩÂäõÈáèÔºåÊàë‰ª¨ÂæàÊ¨¢Ëøé‰Ω†ÁöÑÂä†ÂÖ•„ÄÇ" << endl;
				cout << "ÁõÆÂâçÊãõËÅòÁöÑ‰∏ªË¶Å‰∫∫ÊâçÊòØÔºöËΩ¶ÁâåÂÆö‰ΩçÔºåÂõæÂÉèËØÜÂà´ÔºåÊ∑±Â∫¶Â≠¶‰π†ÔºåÁΩëÁ´ôÂª∫ËÆæÁõ∏ÂÖ≥ÊñπÈù¢ÁöÑÁâõ‰∫∫„ÄÇ" << endl;
				cout << "Â¶ÇÊûú‰Ω†ËßâÂæóËá™Â∑±Á¨¶ÂêàÊù°‰ª∂ÔºåËØ∑ÂèëÈÇÆ‰ª∂Âà∞Âú∞ÂùÄ(easypr_dev@163.com)ÔºåÊúüÂæÖ‰Ω†ÁöÑÂä†ÂÖ•ÔºÅ" << endl;
				cout << endl;
				break;
			case 7:
				// ÊÑüË∞¢ÂêçÂçï
				cout << endl;
				cout << "Êú¨È°πÁõÆÂú®Âª∫ËÆæËøáÁ®ã‰∏≠ÔºåÂèóÂà∞‰∫ÜÂæàÂ§ö‰∫∫ÁöÑÂ∏ÆÂä©ÔºåÂÖ∂‰∏≠‰ª•‰∏ãÊòØÂØπÊú¨È°πÁõÆÂÅöÂá∫Á™ÅÂá∫Ë¥°ÁåÆÁöÑ" << endl;
				cout << "(Ë¥°ÁåÆÂåÖÊã¨ÊúâÁõäÂª∫ËÆÆÔºå‰ª£Á†ÅË∞É‰ºòÔºåÊï∞ÊçÆÊèê‰æõÁ≠âÁ≠â,ÊéíÂêçÊåâÊó∂Èó¥È°∫Â∫è)Ôºö" << endl;
				cout << "taotao1233ÔºåÂîêÂ§ß‰æ†ÔºåjsxyheluÔºåÂ¶ÇÊûúÊúâ‰∏ÄÂ§©(zhoushiwei)ÔºåÂ≠¶‰π†Â•ãÊñóÔºåË¢ÅÊâøÂøóÔºåÂú£ÂüéÂ∞èÁü≥Âå†ÔºågoldriverÔºåMicoozÔºåÊ¢¶ÈáåÊó∂ÂÖâÔºåRain WangÔºå" << endl;
				cout << "ËøòÊúâÂæàÂ§öÁöÑÂêåÂ≠¶ÂØπÊú¨È°πÁõÆ‰πüÁªô‰∫à‰∫ÜÈºìÂä±‰∏éÊîØÊåÅÔºåÂú®Ê≠§‰πü‰∏ÄÂπ∂Ë°®Á§∫ÁúüËØöÁöÑË∞¢ÊÑèÔºÅ" << endl;
				cout << endl;
				break;
			case 8:
				isExit = true;
				break;
			default:
				cout << "ËæìÂÖ•ÈîôËØØÔºåËØ∑ÈáçÊñ∞ËæìÂÖ•:";
				isRepeat = true;
				break;
			}
		}
	}
	return 0;
}
// /EasyPR ËÆ≠ÁªÉÂëΩ‰ª§Ë°å ÁªìÊùü
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// acurayTestMain ÂëΩ‰ª§Ë°å 

const string acuray_option[] = 
	{
		"1. general_test;"	,
		"2. native_test;"	,
		"3. ËøîÂõû;"			,  
	};

const int acuray_optionCount = 3;

int acurayTestMain()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "BatchTest Option:"<< endl;
		for(int i = 0; i < acuray_optionCount; i++)
		{
			selectOption << acuray_option[i] << endl;
		}

		cout << "////////////////////////////////////"<< endl;
		cout << selectOption.str();
		cout << "////////////////////////////////////"<< endl;
		cout << "ËØ∑ÈÄâÊã©‰∏ÄÈ°πÊìç‰Ωú:";

		int select = -1;
		bool isRepeat = true;
		while (isRepeat)
		{
			cin >> select;
			isRepeat = false;
			switch (select)
			{
			case 1:
				acurayTest(GENERAL_TEST_PATH);
				break;
			case 2:
				acurayTest(NATIVE_TEST_PATH);
				break;
			case 3:
				isExit = true;
				break;
			default:
				cout << "ËæìÂÖ•ÈîôËØØÔºåËØ∑ÈáçÊñ∞ËæìÂÖ•:";
				isRepeat = true;
				break;
			}
		}
	}
	return 0;
}

// acurayTestMain ÂëΩ‰ª§Ë°å 
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// SVM ËÆ≠ÁªÉÂëΩ‰ª§Ë°å 

const string svm_option[] = 
	{
		"1. ÁîüÊàêlearndata(Ë∞ÉÊï¥‰ª£Á†ÅÂà∞‰Ω†ÁöÑÁéØÂ¢ÉÂêéÂÜçÁî®);"	,
		"2. Ê†áÁ≠ælearndata;"		,
		"3. ËΩ¶ÁâåÊ£ÄÊµã(not divide and train);"		,
		"4. ËΩ¶ÁâåÊ£ÄÊµã(not train);"		,
		"5. ËΩ¶ÁâåÊ£ÄÊµã(not divide);"		,
		"6. ËΩ¶ÁâåÊ£ÄÊµã;"		,
		"7. ËøîÂõû;"			,  
	};

const int svm_optionCount = 7;

int svmMain()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "SvmTrain Option:"<< endl;
		for(int i = 0; i < svm_optionCount; i++)
		{
			selectOption << svm_option[i] << endl;
		}

		cout << "////////////////////////////////////"<< endl;
		cout << selectOption.str();
		cout << "////////////////////////////////////"<< endl;
		cout << "ËØ∑ÈÄâÊã©‰∏ÄÈ°πÊìç‰Ωú:";

		int select = -1;
		bool isRepeat = true;
		while (isRepeat)
		{
			cin >> select;
			isRepeat = false;
			switch (select)
			{
			case 1:
				getLearnData();
				break;
			case 2:
				label_data();
				break;
			case 3:
				svmTrain(false, false);
				break;
			case 4:
				svmTrain(true, false);
				break;
			case 5:
				svmTrain(false, true);
				break;
			case 6:
				svmTrain();
				break;
			case 7:
				isExit = true;
				break;
			default:
				cout << "ËæìÂÖ•ÈîôËØØÔºåËØ∑ÈáçÊñ∞ËæìÂÖ•:";
				isRepeat = true;
				break;
			}
		}
	}
	return 0;
}

// SVM ËÆ≠ÁªÉÂëΩ‰ª§Ë°å 
////////////////////////////////////////////////////////////

// command line option support

// the function declaration to be used,
// owing to the modules of the current EasyPR is separated, this is only temporary.

int test_plate_locate();
int test_plate_judge();
int test_chars_segment();
int test_chars_identify();
int test_plate_detect();
int test_chars_recognise();
int test_plate_recognize();

void cmdMain(int argc, char *argv[])
{
    const char *help[] = {
        "EasyPR Usage:                                             ",
        "--help                 [ -h   ]    œ‘ æ∞Ô÷˙                ",
        "≤‚ ‘ƒ£øÈ                                                   ",
        "--test_plate_locate    [ -tpl ]    ≥µ≈∆∂®Œª                ",
        "--test_plate_judge     [ -tpj ]    ≥µ≈∆≈–∂œ                ",
        "--test_plate_detect    [ -tpd ]    ≥µ≈∆ºÏ≤‚                ",
        "--test_chars_segment   [ -tcs ]    ◊÷∑˚∑÷∏Ù                ",
        "--test_chars_identify  [ -tci ]    ◊÷∑˚º¯±                ",
        "--test_chars_recognize [ -tcr ]    ◊÷∑˚ ∂±                ",
        "--test_plate_recognize [ -tpr ]    ≥µ≈∆ ∂±                ",
        "--test_all             [ -ta  ]    ≤‚ ‘»´≤ø                ",
        "--general_test         [ -gt  ]    ≈˙¡ø≤‚ ‘-general_test   ",
        "--native_test          [ -nt  ]    ≈˙¡ø≤‚ ‘-native_test    ",
        "SVM—µ¡∑",
        "--svm_gen_learndata    [ -sgl ]    …˙≥…Learndata           ",
        "--svm_tag_learndata    [ -stl ]    ±Í«©Learndata           ",
        "--svm_detect           [ -sd  ]    ≥µ≈∆ºÏ≤‚£¨ø……Ë÷√-vªÚ-t    ",
        "--svm_divide           [ -v   ]    ∆Ù”√∑÷∏Ó                ",
        "--svm_train            [ -t   ]    ∆Ù”√—µ¡∑                ",
        "ANN—µ¡∑£®Œ¥ø™∑≈£©                                           ",
        "GDTS…˙≥…                                                   ",
        "--gdts                 [ -gts  ]   GDTS…˙≥…                ",
        "--group                            ø™∑¢Õ≈∂”                ",
        "--thanks                           ∏––ª√˚µ•                ",
        "                                                          ",
        "Examples:                                                 ",
        "   $ ./EasyPR --test_plate_locate                         ",
        "Will do the same as followings:                           ",
        "   $ ./EasyPR -tpl                                        ",
        "   $ ./EasyPR -t -p -l                                    ",
        "   $ ./EasyPR -ptl                                        ",
        "   $ ./EasyPR -p -l -t                                    ",
        NULL
    };
    
    CParser parser(argc, argv);
    
    try {

        do {
            if (parser.has_or(2, "help", "h") /* || argc < 2 */) {
                int i = 0;
                while (help[i]) {
                    cout << help[i++] << endl;
                }
                break;
            }
            
            // tests
            
            if (parser.has_or(2, "test_plate_locate", "-tpl")) {
                cout << (test_plate_locate() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_plate_judge", "-tpj")) {
                cout << (test_plate_judge() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_plate_detect", "-tpd")) {
                cout << (test_plate_detect() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_chars_segment", "-tcs")) {
                cout << (test_chars_segment() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_chars_identify", "-tci")) {
                cout << (test_chars_identify() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_chars_recognize", "-tcr")) {
                cout << (test_chars_recognise() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_plate_recognize", "-tpr")) {
                cout << (test_plate_recognize() == 0 ? "passed" : "failed");
                break;
            }
            
            if (parser.has_or(2, "test_all", "-ta")) {
                cout << "test_plate_locate "    << (test_plate_locate()    == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_judge "     << (test_plate_judge()     == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_segment "   << (test_chars_segment()   == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_identify "  << (test_chars_identify()  == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_detect "    << (test_plate_detect()    == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_recognize " << (test_chars_recognise() == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_recognize " << (test_plate_recognize() == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_locate "    << (test_plate_locate()    == 0 ? "passed" : "failed") << endl;
                break;
            }
            
            // batch testing
            
            if (parser.has_or(2, "general_test", "-gt")) {
                acurayTest(GENERAL_TEST_PATH);
                break;
            }
            
            if (parser.has_or(2, "native_test", "-nt")) {
                acurayTest(NATIVE_TEST_PATH);
                break;
            }
            
            // svm trains
            
            if (parser.has_or(2, "svm_gen_learndata", "-sgl")) {
                getLearnData();
                break;
            }
            
            if (parser.has_or(2, "svm_tag_learndata", "-stl")) {
                label_data();
                break;
            }
            
            if (parser.has_or(2, "svm_detect", "-sd")) {
                svmTrain(parser.has_or(2, "v", "svm_divide"), parser.has_or(2, "t", "svm_train"));
                break;
            }

            // GDTS
            
            if (parser.has_or(2, "gdts", "gts")) {
                general_test();
                break;
            }
            
            // 
            
            if (parser.has("group")) {
                // ø™∑¢Õ≈∂”;
				cout << endl;
				cout << "Œ“√«EasyPRÕ≈∂”ƒø«∞”–“ª∏ˆ5»À◊Û”“µƒ–°◊È‘⁄Ω¯––EasyPR∫Û–¯∞Ê±æµƒø™∑¢π§◊˜°£" << endl;
				cout << "»Áπ˚ƒ„∂‘±æœÓƒø∏––À»§£¨≤¢«“‘∏“‚Œ™ø™‘¥π±œ◊“ª∑›¡¶¡ø£¨Œ“√«∫‹ª∂”≠ƒ„µƒº”»Î°£" << endl;
				cout << "ƒø«∞’–∆∏µƒ÷˜“™»À≤≈ «£∫≥µ≈∆∂®Œª£¨ÕºœÒ ∂±£¨…Ó∂»—ßœ∞£¨Õ¯’æΩ®…Ëœ‡πÿ∑Ω√Êµƒ≈£»À°£" << endl;
				cout << "»Áπ˚ƒ„æıµ√◊‘º∫∑˚∫œÃıº˛£¨«Î∑¢” º˛µΩµÿ÷∑(easypr_dev@163.com)£¨∆⁄¥˝ƒ„µƒº”»Î£°" << endl;
				cout << endl;
                break;
            }
            
            if (parser.has("thanks")) {
                // ∏––ª√˚µ•
				cout << endl;
				cout << "±æœÓƒø‘⁄Ω®…Ëπ˝≥Ã÷–£¨ ‹µΩ¡À∫‹∂‡»Àµƒ∞Ô÷˙£¨∆‰÷–“‘œ¬ «∂‘±æœÓƒø◊ˆ≥ˆÕª≥ˆπ±œ◊µƒ" << endl;
				cout << "(π±œ◊∞¸¿®”–“ÊΩ®“È£¨¥˙¬Îµ˜”≈£¨ ˝æ›Ã·π©µ»µ»,≈≈√˚∞¥ ±º‰À≥–Ú)£∫" << endl;
				cout << "taotao1233£¨Ã∆¥Ûœ¿£¨jsxyhelu£¨»Áπ˚”–“ªÃÏ(zhoushiwei)£¨—ßœ∞∑‹∂∑£¨‘¨≥–÷æ£¨ •≥«–° ØΩ≥£¨goldriver£¨Micooz£¨√Œ¿Ô ±π‚£¨Rain Wang£¨" << endl;
				cout << "ªπ”–∫‹∂‡µƒÕ¨—ß∂‘±æœÓƒø“≤∏¯”Ë¡Àπƒ¿¯”Î÷ß≥÷£¨‘⁄¥À“≤“ª≤¢±Ì æ’Ê≥œµƒ–ª“‚£°" << endl;
				cout << endl;
				break;
            }
            
        } while(false);
        
    } catch (const std::exception &err) {
        cout << err.what() << endl;
    }
}
