#ifndef __CHARSET_H__
#define __CHARSET_H__

namespace dbwsim
{
  const int NUMBER_OF_CHARS = 256;
  
  class CharSet 
  {
    // set of characters
    unsigned char _set[NUMBER_OF_CHARS];    
  public:
    unsigned char includes(unsigned char c) const 
    {
      return _set[c];
    }
    
    unsigned char val(unsigned char c) const 
    {
      return _set[c];
    }
    
    void set(unsigned char c, unsigned char val);
    
    void init(const char* chars);
    
    void copy(const CharSet& charset);
    
    void neg();
    
    CharSet(const char* chars = 0);
    
    CharSet(const CharSet& charset) 
    {
      copy(charset);
    }
  };

  typedef CharSet* CharSetP;
}

#endif /* __CHARSET_H__ */

