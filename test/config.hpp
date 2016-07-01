#ifndef EASYPR_CONFIG_HPP
#define EASYPR_CONFIG_HPP

#include <vector>

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  namespace demo {

    class Config {
    public:
      Config() {

      }

      Config(const Config& other) {
        m_param1f.assign(other.m_param1f.begin(), other.m_param1f.end());
        m_param2f.assign(other.m_param2f.begin(), other.m_param2f.end());
        m_param3f.assign(other.m_param3f.begin(), other.m_param3f.end());

        m_param1i.assign(other.m_param1i.begin(), other.m_param1i.end());
        m_param2i.assign(other.m_param2i.begin(), other.m_param2i.end());
        m_param3i.assign(other.m_param3i.begin(), other.m_param3i.end());

        m_param1b.assign(other.m_param1b.begin(), other.m_param1b.end());
        m_param2b.assign(other.m_param2b.begin(), other.m_param2b.end());
        m_param3b.assign(other.m_param3b.begin(), other.m_param3b.end());
      }

      inline void setParam1f(float* param, int size) { m_param1f.assign(param, param + size); }
      inline std::vector<float>& getParam1f()  { return m_param1f; }

      inline void setParam2f(float* param, int size) { m_param2f.assign(param, param + size); }
      inline std::vector<float>& getParam2f()  { return m_param2f; }

      inline void setParam3f(float* param, int size) { m_param3f.assign(param, param + size); }
      inline std::vector<float>& getParam3f()  { return m_param3f; }

      inline void setParam1i(int* param, int size) { m_param1i.assign(param, param + size); }
      inline std::vector<int>& getParam1i()  { return m_param1i; }

      inline void setParam2i(int* param, int size) { m_param2i.assign(param, param + size); }
      inline std::vector<int>& getParam2i()  { return m_param2i; }

      inline void setParam3i(int* param, int size) { m_param3i.assign(param, param + size); }
      inline std::vector<int>& getParam3i()  { return m_param3i; }

      inline void setParam1b(bool* param, int size) { m_param1b.assign(param, param + size); }
      inline std::vector<bool>& getParam1b()  { return m_param1b; }

      inline void setParam2b(bool* param, int size) { m_param2b.assign(param, param + size); }
      inline std::vector<bool>& getParam2b()  { return m_param2b; }

      inline void setParam3b(bool* param, int size) { m_param3b.assign(param, param + size); }
      inline std::vector<bool>& getParam3b()  { return m_param3b; }

    private:
      //! float params
      std::vector<float> m_param1f;
      std::vector<float> m_param2f;
      std::vector<float> m_param3f;

      //! int params
      std::vector<int> m_param1i;
      std::vector<int> m_param2i;
      std::vector<int> m_param3i;

      //! bool params
      std::vector<bool> m_param1b;
      std::vector<bool> m_param2b;
      std::vector<bool> m_param3b;
    };

} /*! \namespace demo*/

} /*! \namespace easypr*/

#endif  // EASYPR_CONFIG_HPP