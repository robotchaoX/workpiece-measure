#ifndef IMAGES_CHANGE_H
#define IMAGES_CHANGE_H

class Images_Change
{
public:
    Images_Change();
    char *lightChange(int index);
    /*报警灯图片的路径*/
    char* lights[2] = {":/image/image/error1.png",":/image/image/error2.png"};

};

#endif // IMAGES_CHANGE_H
