#include "test1.h"
#include <iostream>
#include <string>
#include <QString>
#include <QDebug>
#include <QtCore>
#include <QIODevice>
#include <QSerialPort>
#include <stdio.h>
#include <ThreadPool.h>
#include <functional>
#include <map>
#include <utility>
//using namespace std;

typedef unsigned char BYTE;
typedef unsigned char BCD;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#define U8   1
#define U16   2
#define U32   3

/****************************************
 * name:    充电接口字段填充test
 * arg:     按照键-值填充2个list
 * Feature: string:"{\"MethodName\":\"ChargingInfo\",\"args\":{  \"realtimePower\":\"3000\"  , \"powerLevelPrc\":\"4000\" }}"
 * **************************************/
int feedFormatStr(std::list<std::string> &name, std::list<std::string> &arg)
{
    name.emplace_back("realtimePower");
    arg.emplace_back("100");
    name.emplace_back("powerLevelPrc");
    arg.emplace_back("101");
    name.emplace_back("chargingState");
    arg.emplace_back("1");
    name.emplace_back("chargingGunState");
    arg.emplace_back("1");
    name.emplace_back("fuelRange");
    arg.emplace_back("60000");
    name.emplace_back("chargingType");
    arg.emplace_back("7");
    name.emplace_back("chargingTimeLevelPrc");
    arg.emplace_back("20000");
    name.emplace_back("startTime");
    arg.emplace_back("2147483600");
    name.emplace_back("endTime");
    arg.emplace_back("2147483640");
    name.emplace_back("chargingCurrent");
    arg.emplace_back("20000");
    name.emplace_back("chargingVoltage");
    arg.emplace_back("380");
    name.emplace_back("chargingPileID");
    arg.emplace_back("60");
    name.emplace_back("chargingPileSupplier");
    arg.emplace_back("61");
    name.emplace_back("workingCurrent");
    arg.emplace_back("200");
    name.emplace_back("workingVoltage");
    arg.emplace_back("201");
    name.emplace_back("mileageSinceLastCharge");
    arg.emplace_back("35000");
    name.emplace_back("powerUsageSinceLastCharge");
    arg.emplace_back("15000");
    name.emplace_back("mileageOfDay");
    arg.emplace_back("1000");
    name.emplace_back("powerUsageOfDay");
    arg.emplace_back("25000");
    name.emplace_back("staticEnergyConsumption");
    arg.emplace_back("3500");
    name.emplace_back("chargingElectricityPhase");
    arg.emplace_back("2");
    name.emplace_back("chargingDuration");
    arg.emplace_back("123456789");
    name.emplace_back("lastChargeEndingPower");
    arg.emplace_back("65534");
    name.emplace_back("totalBatteryCapacity");
    arg.emplace_back("60000");
    name.emplace_back("fotaLowestVoltage");
    arg.emplace_back("150");
    name.emplace_back("mileage");
    arg.emplace_back("123456789");
    name.emplace_back("extendedData1");
    arg.emplace_back("25000");
    name.emplace_back("extendedData2");
    arg.emplace_back("15000");
    name.emplace_back("extendedData3");
    arg.emplace_back("225");
    name.emplace_back("extendedData4");
    arg.emplace_back("226");

}

/****************************************
 * name:    numString转uint
 * arg:     type 数据类型， pos 转换的起点  off 转化长度
 * Feature:
 * **************************************/
uint32_t numString2UInt(const std::string str , char type, int pos = -1, int off =-1)
{
    uint32_t sum = 0;
    uint32_t upperLimit = 0;
    uint32_t lowerLimit = 0;
    std::string strTemp = str;

    uint32_t strlen = str.size();
    if(str.size() == 0)
        return -1;

    if((pos!=-1)&&(off!=-1))
    {
        if((pos+off)<=strlen)
        {
            strTemp = str.substr(pos,off);
            strlen = strTemp.size();
        }
        else
        {
            return -1;
        }
    }

    switch (type) {
    case 1:     //U8
        upperLimit = 255;
        lowerLimit = 0;
        break;
    case 2:     //u16
        upperLimit = 65535;
        lowerLimit = 0;
        break;
    case 3:     //u32
        upperLimit = 2147483647;
        lowerLimit = 0;
        break;
    default:
        return -1;
        break;
    }
    printf("strTemp.size() = %ld \n", strTemp.size());
    for(int i=0; i<strlen; i++)
    {
        if((strTemp.at(i)>= '0') && (strTemp.at(i)<= '9'))
        {
            if(i!=0)
                sum *=10;
            sum += (strTemp.at(i) - '0');
        }
//        printf("sum = %d \n", sum);
        if((sum < lowerLimit) || (sum > upperLimit))
            return -1;
    }
    return sum;
}

/****************************************
 * name:    DBUS格式化字符串中arg段的构造
 * Feature: 构造长度限定 1K
 * **************************************/
int formatStr(std::list<std::string> name, std::list<std::string> arg)
{
    int cnt=0;
    char buff[1024*1024] = {0};
    if(name.size() != arg.size())
        return -1;
    char* p = NULL;
    p = buff;
    std::list<std::string>::iterator it;
    std::list<std::string>::iterator it2;

    it2 = arg.begin();
    for ( it=name.begin() ; it != name.end(); it++,it2++ )
    {

        sprintf(p+strlen(p),"\\\"%s\\\":\\\"%s\\\", ",it->c_str(), it2->c_str());
        cnt++;
    }
    sprintf(p+strlen(p)-2,"}}\""); //这个是减去 空格

    printf("%s\n",buff);
    printf("cnt = %d\n",cnt);
    return 1;
}

/****************************************
 * name:    str转float
 * Feature: 精度小数点后6位，这种方式没有高16位整数，低16位小数那样精度高
 * **************************************/
double numString2double(const std::string str )
{
    double sum = 0.0;
    double decimals = 10.0;
    int sta = 0;
    std::string strTemp = str;
    uint32_t strlen = strTemp.size();
    if((strlen == 0) || (strlen>=13))
        return -1.0;
//    printf("strTemp.size() = %d \n", strTemp.size());
    for(int i=0; i<strlen; i++)
    {
        if((strTemp.at(i)>= '0') && (strTemp.at(i)<= '9') && (sta == 0))
        {
            if(i!=0)
                sum *=10.0;
            sum += (double)(strTemp.at(i) - '0');
        }
        else if(strTemp.at(i) == '.')
        {
            sta = 1;
        }
        else if((strTemp.at(i)>= '0') && (strTemp.at(i)<= '9') && (sta == 1))
        {
            sum += (double)(strTemp.at(i) - '0')/decimals;
            decimals *= 10.0;
        }
    }
    return sum;
}

/****************************************
 * name:    比较内存块数据
 * Feature: 判断array，然后刷新
 * **************************************/
bool checkArraySame(uint8_t* pcan_shm_mVal, uint8_t* value, size_t n)
{
    bool ret = false;
    int sta=0;
    sta = memcmp((void*)pcan_shm_mVal,(void*)value,n);
    if(sta != 0)
    {
        memcpy((void*)pcan_shm_mVal, (void*)value, n);
        ret = true;
    }
    return ret;
}

void func1(int a)
{cout<<"hello = "<<a<<endl;}


// 0x12 0x34 0x56 -> "123456"
string hexEx2string(uint8_t* data, uint32_t len)
{
    uint8_t hbit = 0;
    uint8_t lbit = 0;
    string str;
    uint32_t i = 0;
    char buf[100]={0};
    for(i=0; i<len; i++)
    {
        hbit = (0xf0 & data[i])>>4;
        lbit = (0x0f & data[i]);
        sprintf(buf,"%01x%01x",hbit,lbit);
        str += buf;
    }
    return str;
}

//  "123456" -> 0x12 0x34 0x56
uint8_t* string2hexEx(uint8_t* dest, const char* src)
{
    uint8_t hbit = 0;
    uint8_t lbit = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t len = 0;
    len = strlen(src);
    std::cout<<"len = "<<len <<std::endl;
    for(i=0; i<len; i=i+2)
    {
        hbit = *(src+i);
        if((hbit>='0') && (hbit<='9'))
        {
            hbit = hbit - '0';
        }
        else if((hbit>='a') && (hbit<='f'))
        {
            hbit = hbit - 'a'+ 0x0a;
        }
        else if((hbit>='A') && (hbit<='F'))
        {
             hbit = hbit - 'A'+ 0x0a;
        }
        else
        {
            printf("string2hexEx error ");
            return dest = NULL;
        }

        hbit = (uint8_t)(hbit<<4);
//        printf("%x ",hbit);
        lbit = *(src+i+1);
        if((lbit>='0') && (lbit<='9'))
        {
            lbit = lbit - '0';
        }
        else if((lbit>='a') && (lbit<='f'))
        {
            lbit = lbit - 'a'+ 0x0a;
        }
        else if((lbit>='A') && (lbit<='F'))
        {
             lbit = lbit - 'A'+ 0x0a;
        }
        else
        {
            printf("string2hexEx error ");
            return dest = NULL;
        }
        dest[j++] = hbit+lbit;
    }
    return dest;
}

//  "123456" -> 0x12 0x34 0x56
uint8_t* string2hexEx(uint8_t* dest, string& src)
{
    uint8_t hbit = 0;
    uint8_t lbit = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t len = 0;
    len = src.length();
    string str;
//    std::cout<<"len = "<<len <<std::endl;
    for(i=0; i<len; i=i+2)
    {
        str = src.substr(i,1);
        hbit = std::stoi(str,0,16);
        hbit = (uint8_t)(hbit<<4);

        str = src.substr(i+1,1);
        lbit = std::stoi(str,0,16);
        dest[j++] = hbit+lbit;
    }
    return dest;
}





int main(int argc,char**argv)
{

    QString h("hello Qt5\n");
    std::cout<<h.toStdString();
    std::cout<<"hello world!"<<std::endl;

    test1 t1;
    t1.print();
    char arry[2]={0};
    arry[0] = 'a';
    printf("atoi = %x \n",std::atoi(&arry[0])); //居然等于 0




    return 1;


//    string str1 = "0f2a3944";
//    uint8_t buf[8] = {0};

//    printf("str1.length() = %ld \n",str1.length());
//    printf("str1.size() = %ld \n",str1.size());

////    string2hexEx(buf, str1.c_str());
//    string2hexEx(buf,str1);

//    printf("buff  ");
//    for(int i=0; i<4 ; i++)
//        printf("%02x ",buf[i]);

//    printf("\n ");


//    uint8_t buf2[4] = {0x0f,0x2a,0x39,0x44};
//    string str2;
//    str2 = hexEx2string(buf2,4);
//    printf("str2 = %s \n", str2.c_str());
//    return 1;

    std::map<string , int> mapTemp;
    mapTemp.emplace(std::make_pair("A",1));
    mapTemp.emplace(std::make_pair("B",2));
    mapTemp.emplace(std::make_pair("C",3));
    mapTemp.emplace(std::make_pair("D",4));
    if (mapTemp.find("D") != mapTemp.end()) {
        printf("xxxxx");

//        map<string, int>::iterator  iter ;
//         for(iter = mapTemp.begin(); iter != mapTemp.end(); ++iter)
//         {
//           if(iter->first=="D" ) {
//                printf("\n  %d \n",iter->second);
//           }
//         }
        map<string, int>::iterator  iter = mapTemp.find("D");
//        auto iter = mapTemp.find("C");
            printf("\n  %d \n", iter->second );


    }
    else
    {
        printf("yyyy");
    }


    /****************************************
 * name:    cpp中函数指针的绑定
 * Feature: 熟悉cpp和c中函数指针使用上的差异
 * **************************************/
    //    using My_func = std::function<void(int a)>;
    //    My_func func=func1;
    //    func(1100);

//    std::function<void()> f_display_31337 = std::bind(func1, 31337);
//      f_display_31337();




    /****************************************
     * name:    Cpp的String测试
     * Feature: substr获取子串的测试
     * **************************************/
    //    char a[20]="yxyxyxyx  55";
    //    std::string tt;
    //    tt = a;
    //    tt = tt.substr(0,2);
    //    std::cout<<tt<<std::endl;

    /****************************************
     * name:    串口通信中COBS的测试
     * Feature: 串口发送和接收的核心，报文的转译
     * **************************************/
//    unsigned char inData[10] = {0x11,0x22,0x00,0xFF};
//    unsigned char outData[10]={0};
//    unsigned char packData[10] = {0};

//    StuffData(inData,4,outData);

//    for(int i=0;i<10;i++)
//        printf("%02x ", outData[i]);
//        printf("\n");

//    UnStuffData(outData,strlen((const char*)outData),packData);

//    for(int i=0;i<10;i++)
//        printf("%02x ", packData[i]);
//        printf("\n");

    /****************************************
     * name:    Cpp中数字转字符串
     * Feature:
     * **************************************/
//    char ii = 2;
//    long longi = 123456789;
//    t1.str1 = std::to_string(longi);
//	std::cout<<t1.str1<<std::endl;

    /****************************************
     * name:    c++标准list遍历
     * Feature:
     * **************************************/
//    std::list<std::string> lt;
//    lt.emplace_back("aaaa");
//    lt.emplace_back("bbbb");
//    for(const auto& name : lt)
//        std::cout << name << std::endl; //遍历list，放在name中

//    std::list<std::string> blist ;
//    blist.emplace_back("xxxxxx");
//    blist.emplace_back("yyyyyy");
//    std::list<std::string>::iterator it;
//    for ( it=blist.begin() ; it != blist.end(); it++ )
//    {
//         std::cout <<*it <<std::endl; //依次取list里面的值到temp中
//    }



    /****************************************
     * name:    string 的使用
     * Feature:
     * **************************************/
//    std::string testStr;
//    testStr = "123456789";
//    std::cout <<"testStr.at(0)"<<testStr.at(0)<<std::endl;


//    //float
//    testStr = "127.123456711111";
//    printf("testStr =  %f \n", numString2double(testStr));

    //int
//    std::string testStr;
//    testStr = "1";
//    printf("testStr =  %d \n", (unsigned char)numString2UInt(testStr,U32,0,1));

//    //array对比
//    uint8_t array1[10] = {1,2,3,4};
//    uint8_t array2[10] = {1,2,3,4,-1};
//    printf ("checkArraySame = %d\n",checkArraySame(array1,array2,5));


//    json充电接口字段填充
//    std::list<std::string> name;
//    std::list<std::string> arg;
//    feedFormatStr(name,arg);
//    formatStr(name,arg);
}








