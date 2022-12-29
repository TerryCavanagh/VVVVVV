#ifndef LABCLASS_H
#define LABCLASS_H

class labclass
{
public:
    const short* loadlevel(int rx, int ry);

    const char* roomname;
    bool roomname_special;
    int rcol;
};
#endif /* LABCLASS_H */
