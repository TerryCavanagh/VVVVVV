#ifndef LABCLASS_H
#define LABCLASS_H

#include <string>

class labclass
{
public:
    const short* loadlevel(int rx, int ry);

    std::string roomname;
    int rcol;
};
#endif /* LABCLASS_H */
