#ifndef EASYPR_RESULT_HPP
#define EASYPR_RESULT_HPP

#include <vector>

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  namespace demo {

    class Result {

      class Params {
      public:
        Params() {

        }

        Params(const Params& other) {

          m_param1f = other.m_param1f;
          m_param2f = other.m_param2f;
          m_param3f = other.m_param3f;

          m_param1i = other.m_param1i;
          m_param2i = other.m_param2i;
          m_param3i = other.m_param3i;

          m_param1b = other.m_param1b;
          m_param2b = other.m_param2b;
          m_param3b = other.m_param3b;
        }

        inline void setParam1f(float param) { m_param1f = param; }
        inline float getParam1f() const { return m_param1f; }

        inline void setParam2f(float param) { m_param2f = param; }
        inline float getParam2f() const { return m_param2f; }

        inline void setParam3f(float param) { m_param3f = param; }
        inline float getParam3f() const { return m_param3f; }

        inline void setParam1i(int param) { m_param1i = param; }
        inline int getParam1i() const { return m_param1i; }

        inline void setParam2i(int param) { m_param2i = param; }
        inline int getParam2i() const { return m_param2i; }

        inline void setParam3i(int param) { m_param3i = param; }
        inline int getParam3i() const { return m_param3i; }

        inline void setParam1b(bool param) { m_param1b = param; }
        inline bool getParam1b() const { return m_param1b; }

        inline void setParam2b(bool param) { m_param2b = param; }
        inline bool getParam2b() const { return m_param2b; }

        inline void setParam3b(bool param) { m_param3b = param; }
        inline bool getParam3b() const { return m_param3b; }

        friend std::ostream& operator << (std::ostream& os, const Params& obj)
        {
          // write obj to stream
          os << "m_param1f" << ":" << obj.m_param1f << ",  ";
          os << "m_param2f" << ":" << obj.m_param2f << ",  ";
          os << "m_param3f" << ":" << obj.m_param3f << ",  ";
          os << std::endl;

          os << "m_param1i" << ":" << obj.m_param1i << ",  ";
          //os << "m_param2i" << ":" << obj.m_param2i << ",  ";
          //os << "m_param3i" << ":" << obj.m_param3i << ",  ";
          os << std::endl;

          //os << "m_param1b" << ":" << obj.m_param1b << ",  ";
          //os << "m_param2b" << ":" << obj.m_param2b << ",  ";
          //os << "m_param3b" << ":" << obj.m_param3b << ",  ";
          //os << std::endl;

          return os;
        }

      private:
        //! float params
        float m_param1f;
        float m_param2f;
        float m_param3f;

        //! int params
        int m_param1i;
        int m_param2i;
        int m_param3i;

        //! bool params
        bool m_param1b;
        bool m_param2b;
        bool m_param3b;
      };

    public:
      Result() {

      }

      Result(const Result& other) {
        m_picturesCount = other.m_picturesCount;
        m_platesCount = other.m_platesCount;
        m_detectRate = other.m_detectRate;
        m_detectRecall = other.m_detectRecall;
        m_detectPrecise = other.m_detectPrecise;
        m_detectFscore = other.m_detectFscore;
        m_0error = other.m_0error;
        m_1error = other.m_1error;
        m_chinesePrecise = other.m_chinesePrecise;

        m_seconds = other.m_seconds;
        m_avgSeconds = other.m_avgSeconds;
        m_params = other.m_params;
      }

      inline void setPicturesCount(int param) { m_picturesCount = param; }
      inline int getPicturesCount() const { return m_picturesCount; }

      inline void setPlatesCount(int param) { m_platesCount = param; }
      inline int getPlatesCount() const { return m_platesCount; }

      inline void setDetectRate(float param) { m_detectRate = param; }
      inline float getDetectRate() const { return m_detectRate; }

      inline void setDetectRecall(float param) { m_detectRecall = param; }
      inline float getDetectRecall() const { return m_detectRecall; }

      inline void setDetectPrecise(float param) { m_detectPrecise = param; }
      inline float getDetectPrecise() const { return m_detectPrecise; }

      inline void setDetectFscore(float param) { m_detectFscore = param; }
      inline float getDetectFscore() const { return m_detectFscore; }

      inline void setZeroError(float param) { m_0error = param; }
      inline float getZeroError() const { return m_0error; }

      inline void setOneError(float param) { m_1error = param; }
      inline float getOneError() const { return m_1error; }

      inline void setChinesePreciese(float param) { m_chinesePrecise = param; }
      inline float getChinesePreciese() const { return m_chinesePrecise; }

      inline void setSeconds(float param) { m_seconds = param; }
      inline float getSeconds() const { return m_seconds; }

      inline void setAvgSeconds(float param) { m_avgSeconds = param; }
      inline float getAvgSeconds() const { return m_avgSeconds; }

      inline void setParams(Params param) { m_params = param; }
      inline Params& getParams()  { return m_params; }

      friend std::ostream& operator<<(std::ostream& os, const Result& obj)
      {
        // write obj to stream
        os << "------------------" << std::endl;
        os << "m_picturesCount" << ":" << obj.m_picturesCount << ",  ";
        os << "m_platesCount" << ":" << obj.m_platesCount << ",  ";
        os << "m_detectRate" << ":" << obj.m_detectRate << "%. " << std::endl;

        os << "m_detectRecall" << ":" << obj.m_detectRecall << "%" << ", ";
        os << "m_detectPrecise" << ":" << obj.m_detectPrecise << "%" << ", ";
        os << "m_detectFscore" << ":" << obj.m_detectFscore << "%" << "." << endl;

        os << "m_0error" << ":" << obj.m_0error << "%,  ";
        os << "m_1error" << ":" << obj.m_1error << "%,  ";
        os << "m_chinesePrecise" << ":" << obj.m_chinesePrecise << "%.  " << endl;

        os << "m_seconds" << ":" << obj.m_seconds << "sec" << ",  ";
        os << "m_avgSeconds" << ":" << obj.m_avgSeconds << "sec." << std::endl;

        os << obj.m_params << std::endl;

        return os;
      }

    private:
      int m_picturesCount;
      int m_platesCount;
      float m_detectRate;

      float m_detectRecall;
      float m_detectPrecise;
      float m_detectFscore;

      float m_0error;
      float m_1error;
      float m_chinesePrecise;

      float m_seconds;
      float m_avgSeconds;

      Params m_params;
    };

    



  } /*! \namespace demo*/

} /*! \namespace easypr*/

#endif  // EASYPR_RESULT_HPP