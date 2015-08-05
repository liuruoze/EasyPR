#include "easypr/train/ann_train.h"

namespace easypr{

  AnnTrain::AnnTrain(const char* chars_folder, const char* zhchars_folder, const char* xml)
    :chars_folder_(chars_folder), zhchars_folder_(zhchars_folder), ann_xml_(xml){

  }

  void AnnTrain::train(const int & neurons /* = 40 */){
    this->getTrainData();

    cv::Mat layers = {
      train_data_->getNSamples(),// the input layer
      neurons, // the neurons
      sizeof(kChinese) + sizeof(kCharacters) // the output layer
    };
    ann_->setLayerSizes(layers);
    ann_->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann_->setBackpropWeightScale(0.1);
    ann_->setBackpropMomentumScale(0.1);

    std::cout << "Training ANN model, please wait..." << std::endl;
    long start = utils::getTimestamp();
    ann_->train(train_data_);
    long end = utils::getTimestamp();
    std::cout << "Training done. Elapse: " << (end - start) / 1000 << std::endl;

    ann_->save(ann_xml_);
    std::cout << "Your ANN Model was saved to " << ann_xml_ << std::endl;
  }

  void AnnTrain::getTrainData(){
    assert(chars_folder_);
    assert(zhchars_folder_);
    // create new
    cv::Mat samples;
    cv::Mat responses;

    std::cout << "Collecting chars in " << chars_folder_ << std::endl;

    for (auto i = 0; i < sizeof(kCharacters); ++i){
      char c = kCharacters[i];
      char sub_folder[512] = { 0 };

      sprintf(sub_folder, "%s/%c", chars_folder_, c);
      std::cout << "  >> Featuring characters " << c << " in " << sub_folder << std::endl;

      auto chars_files = utils::getFiles(sub_folder);
      for (auto file : chars_files){
        auto img = cv::imread(file);
        auto fps = features(img, kPredictSize);

        samples.push_back(fps);
        responses.push_back(i);
      }
    }

    std::cout << "Collecting zh-chars in " << zhchars_folder_ << std::endl;

    for (auto i = 0; i < sizeof(kChinese); ++i){
      const char *zhc = kChinese[i];
      char sub_folder[512] = { 0 };

      sprintf(sub_folder, "%s/%s", zhchars_folder_, zhc);
      std::cout << "  >> Featuring zh-characters " << zhc << " in " << sub_folder << std::endl;

      auto chars_files = utils::getFiles(sub_folder);
      for (auto file : chars_files){
        auto img = cv::imread(file);
        auto fps = features(img, kPredictSize);

        samples.push_back(fps);
        responses.push_back(i + sizeof(kCharacters));
      }
    }
    //
    cv::Mat samples_row;
    cv::Mat responses_row;

    samples.convertTo(samples_row, CV_32FC1);
    responses.convertTo(responses_row, CV_32FC1);

    train_data_ = cv::ml::TrainData::create(samples_row, ml::SampleTypes::ROW_SAMPLE, responses_row);
  }

}
