//////////////////////////////////////////////////////////////////////////
// Name:	    params Header
// Version:		1.0
// Date:	    2016-07-01
// Author:	    liuruoze
// Copyright:   liuruoze
// Desciption:
// An abstract class for runtime algorithm params in easypr.
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_PARAMS_H_
#define EASYPR_CORE_PARAMS_H_

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  class CParams {
  public:
    static CParams* instance();

    CParams(const CParams& other) {
      m_isUpdate = other.m_isUpdate;

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

    inline void setIsUpdate(bool param) { m_isUpdate = param; }
    inline bool getIsUpdate() const { return m_isUpdate; }

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

  private:
    CParams() {
      m_isUpdate = false;
    }

    static CParams* instance_;

    // weather update;
    bool m_isUpdate;

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

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PARAMS_H_