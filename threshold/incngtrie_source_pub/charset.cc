#include "charset.h"

namespace dbwsim
{
  void CharSet::set(unsigned char c, unsigned char val)
  {    
    _set[c] = val;    
  }

  void CharSet::init(const char* chars)
  {   
    for (int i = 0; i<NUMBER_OF_CHARS; i++)
      _set[i] = 0;
    
    if (chars==0)
      return;
    
    for (const unsigned char* p = (unsigned char*) chars; *p; p++) 
    {      
      if (*(p+1)=='-' && *(p+2)) 
      {        
        unsigned int to = *(p+2);        
        for (unsigned int i = *p; i<=to; i++) 
          _set[i] = 1;        
        p += 2;        
      }            
      else
        _set[*p] = 1;
    }    
  }

  CharSet::CharSet(const char* chars) 
  {    
    init(chars);    
  }

  void CharSet::copy(const CharSet& charset)
  {    
    for (int i = 0; i<NUMBER_OF_CHARS; i++)
      _set[i] = charset._set[i];    
  }  

  void CharSet::neg()
  {
    for (int i = 0; i<NUMBER_OF_CHARS; i++) 
    {
      if (_set[i])
        _set[i] = 0;
      else
        _set[i] = 1;
    }
  }
}
