#include "images_change.h"

/*构造函数*/
Images_Change::Images_Change()
{

}
/*报警灯切换图片的方法*/
char* Images_Change::lightChange(int index){

    return(lights[index]);  //返回图片的路径
}
