#include <Arduino.h>
#include <SPI.h> //导入库
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include <WiFi.h> // for WiFi shield
#include <WiFiUdp.h>
const char *ssid = "SONDER";          // wifi账号
const char *password = "sonderswifi"; // wifi密码
#include <myFont.h>
#include <pic.h>
#include <imgs.h>

TFT_eSPI tft = TFT_eSPI();
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com"); // NTP服务器地址
int nextPage = 0;
// String weekDays[7] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
String weekDay;
char *week;
//定义太空人照片的指针数组
int i = 0;
int pageReturn = 0;
/*******************单个汉字显示****************/
void showMyFont(int32_t x, int32_t y, const char c[3], uint32_t color)
{
  for (int k = 0; k < 25; k++) // 根据字库的字数调节循环的次数
    if (hanzi[k].Index[0] == c[0] && hanzi[k].Index[1] == c[1] &&
        hanzi[k].Index[2] == c[2])
    {
      tft.drawBitmap(x, y, hanzi[k].hz_Id, hanzi[k].hz_width, 16, color);
    }
}
/*******************整句汉字显示****************/
void showMyFonts(int32_t x, int32_t y, const char str[], uint32_t color)
{
  //显示整句汉字，字库比较简单，上下、左右输出是在函数内实现
  int x0 = x;
  for (int i = 0; i < strlen(str); i += 3)
  {
    showMyFont(x0, y, str + i, color);
    x0 += 17;
  }
}
/*******************整句字符串显示****************/
void showtext(int16_t x, int16_t y, uint8_t s, uint16_t fg, uint16_t bg, const String str)
{
  // uint8_t font,
  //设置文本显示坐标，和文本的字体，默认以左上角为参考点，
  tft.setCursor(x, y);
  // 设置文本颜色为白色，文本背景黑色
  tft.setTextColor(fg, bg);
  //设置文本大小，文本大小的范围是1-7的整数
  tft.setTextSize(s);
  // 设置显示的文字，注意这里有个换行符 \n 产生的效果
  tft.println(str);
}

/*******************时间界面显示****************/
void show_time(String currentHourcurrentMinute, String Second, String currentYear, String currentMonthmonthDay, const char *week)
{
  tft.setSwapBytes(true);
  tft.pushImage(60, 55, 64, 64, Astronaut[i]); // (10,55) 显示 64 × 64像素的图片
  delay(50);
  i += 1; //下一帧
  if (i > 8)
  {
    i = 0;
  };
  showtext(20, 10, 3, TFT_WHITE, TFT_BLACK, currentHourcurrentMinute);
  showtext(110, 24, 1, TFT_WHITE, TFT_BLACK, Second);
  showtext(10, 60, 2, TFT_WHITE, TFT_BLACK, currentYear);
  showtext(10, 80, 2, TFT_WHITE, TFT_BLACK, currentMonthmonthDay);
  showMyFonts(10, 100, week, TFT_SKYBLUE);
  delay(100);
}
void get_wifi()
{
  WiFi.begin(ssid, password);  //连接wifi
  tft.fillScreen(TFT_WHITE);   //屏幕颜色
  tft.setTextColor(TFT_BLACK); //设置字体颜色黑色
  tft.setCursor(10, 64, 1);    //设置文字开始坐标(15,30)及1号字体
  tft.setTextSize(1);
  tft.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  };
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE); //设置字体颜色黑色
  tft.setCursor(20, 64, 1);    //设置文字开始坐标(15,30)及1号字体
  tft.setTextSize(1);
  tft.println("WiFi Connected!");
  delay(2000);
}
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);   //屏幕颜色
  tft.setTextColor(TFT_WHITE); //设置字体颜色黑色
  tft.setCursor(15, 30, 1);    //设置文字开始坐标(15,30)及1号字体
  tft.setTextSize(1);
  tft.setRotation(4);

  tft.println("Connecting Wifi...");
  get_wifi();
  timeClient.begin();
  timeClient.setTimeOffset(28800);
  // tft.println("TFT_Text");
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  // put your main code here, to run repeatedly:
  //延时
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime(); //打印时间
  if (nextPage == 0)
  {
    pageReturn = epochTime;
    nextPage = 1;
  }
  struct tm *ptm = gmtime((time_t *)&epochTime);
  String currentHour = String(timeClient.getHours() / 10) + String(timeClient.getHours() % 10);
  String currentMinute = String(timeClient.getMinutes() / 10) + String(timeClient.getMinutes() % 10);
  String currentHourcurrentMinute = currentHour + ":" + currentMinute;
  int currentSecond = timeClient.getSeconds();
  String weekDay = WeekDayArr[timeClient.getDay()];
  char week[weekDay.length() + 1];
  weekDay.toCharArray(week, weekDay.length() + 1);
  String Second = String(currentSecond / 10) + String(currentSecond % 10);
  String currentYear = String(ptm->tm_year + 1900);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon * 1 + 1;
  String currentMonthmonthDay = String(currentMonth) + "/" + String(monthDay);
  if (epochTime - pageReturn >= 10)
  {
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true); //使图片颜色由RGB->BGR
    tft.pushImage(0, 0, 128, 128, images[0]);
    delay(3000);
    tft.pushImage(0, 0, 128, 128, images[1]);
    delay(3000);
    tft.pushImage(0, 0, 128, 128, images[2]);
    delay(3000);
    tft.pushImage(0, 0, 128, 128, images[3]);
    delay(3000);
    tft.fillScreen(TFT_BLACK);
    // showtext(10, 80, 2, TFT_WHITE, TFT_BLACK, currentMonthmonthDay);
    showMyFonts(40, 20, "曹家齐", TFT_WHITE);
    showtext(40, 45, 2, TFT_RED, TFT_BLACK, "LOVE");
    showMyFonts(40, 70, "胡梦真", TFT_WHITE);
    delay(3000);
    pageReturn = timeClient.getEpochTime();
    tft.fillScreen(TFT_BLACK);
  }
  else
  {
    show_time(currentHourcurrentMinute, Second, currentYear, currentMonthmonthDay, week);
  }
}
