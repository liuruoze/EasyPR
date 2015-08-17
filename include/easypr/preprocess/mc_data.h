// learn data_prepare :
// 生成learn data的cpp，learn data指的是train data，verify data， test
// data的总和，这个名字是我起的。
// learn
// data应该是贴好标签的数据，因此即便通过EasyPR自动化处理以后，也要人为的修正一下.

// 你应该按照如下的步骤和顺序组织learn data：
// 1.用EasyPR对not label
// data进行处理，通过EasyPR辅助来对车牌图片进行标签（放到不同文件夹下）；
// 2.标签分两种，一种是有车牌的，一种是无车牌的；
// 2.EasyPR生成标签以后，你还得需要肉眼确认下，将被不正确标签的数据转移到它们该去的文件夹下；
// 3.通过上面的步骤，正确的车牌图片和非车牌图片已经被分别放到两个文件下，假设是hasplate和noplate；
// 4.将这两个文件夹放到EasyPR目录train/data/plate_detect_svm/learn下面
// 5.运行EasyPR，选择“训练”目录下的“车牌检测（not
// divide）”，这个程序会自动把learn data分块，训练，测试

#ifndef EASYPR_CORE_MCDATA_H_
#define EASYPR_CORE_MCDATA_H_

#include <string>

namespace easypr {

namespace preprocess {

void create_learn_data(const char* raw_data_folder, const char* out_data_folder,
                       const int how_many = 5000);

void tag_data(const char* source_folder, const char* has_plate_folder,
              const char* no_plate_folder, const char* svm_model);
}
}

#endif  // EASYPR_CORE_MCDATA_H_
