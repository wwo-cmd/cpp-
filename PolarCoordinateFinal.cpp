#include <iostream>
#include "exprtk.hpp"       //第三方函数解析运算库，编译的时候会有问题。需要输入附加的编译选项：/bigobj
#include "graphics.h"		// 引用图形库头文件easyx
#include <windows.h>
#include <ctime>
using namespace std;
//窗口背景颜色
const COLORREF BKGCOLOR = RGB(255,255,255);
const int ScreenWidth = 1024;
const int ScreenHeight = 800;

// 检测按键，并返回第一个可打印字符 VK
int getPrintableKeyPressed() {
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

    // 字母
    for (int vk = 'A'; vk <= 'Z'; vk++)
        if (GetAsyncKeyState(vk) & 0x8000) return vk;

    // 数字
    for (int vk = '0'; vk <= '9'; vk++)
        if (GetAsyncKeyState(vk) & 0x8000) return vk;

    // 符号键
    int symbols[] = { VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_MINUS, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_PERIOD };
    for (int vk : symbols)
        if (GetAsyncKeyState(vk) & 0x8000) return vk;

    return 0;
}

// 将 VK 转换成对应字符（只处理可打印字符）
char vkToCharPrintable(int vk, bool shiftPressed) {
    // 字母
    if (vk >= 'A' && vk <= 'Z') {
        return shiftPressed ? char(vk) : tolower(vk);
    }

    // 数字
    if (vk >= '0' && vk <= '9') {
        if (shiftPressed) {
            switch (vk) {
            case '1': return '!';
            case '2': return '@';
            case '3': return '#';
            case '4': return '$';
            case '5': return '%';
            case '6': return '^';
            case '7': return '&';
            case '8': return '*';
            case '9': return '(';
            case '0': return ')';
            }
        }
        else return char(vk);
    }

    // 符号键
    if (!shiftPressed) {
        switch (vk) {
        case VK_OEM_1: return ';';
        case VK_OEM_2: return '/';
        case VK_OEM_3: return '`';
        case VK_OEM_4: return '[';
        case VK_OEM_5: return '\\';
        case VK_OEM_6: return ']';
        case VK_OEM_7: return '\'';
        case VK_OEM_MINUS: return '-';
        case VK_OEM_PLUS: return '=';
        case VK_OEM_COMMA: return ',';
        case VK_OEM_PERIOD: return '.';
        }
    }
    else {
        switch (vk) {
        case VK_OEM_1: return ':';
        case VK_OEM_2: return '?';
        case VK_OEM_3: return '~';
        case VK_OEM_4: return '{';
        case VK_OEM_5: return '|';
        case VK_OEM_6: return '}';
        case VK_OEM_7: return '"';
        case VK_OEM_MINUS: return '_';
        case VK_OEM_PLUS: return '+';
        case VK_OEM_COMMA: return '<';
        case VK_OEM_PERIOD: return '>';
        }
    }

    return 0;
}

//自绘制组件基类
class CHControl {
protected:
    int left, top, width, height;    // 矩形坐标
    //bool m_clicked;           // 状态
    // 检查坐标是否在矩形内
public:
    std::string name;      // 按钮名称
    std::string title;
    CHControl(std::string name1, std::string title1, int l, int t, int w, int h) :name(name1), title(title1), left(l), top(t), width(w), height(h) {};
    virtual bool isPointInButton(int mx, int my)const {
        return mx >= left && mx <= left + width && my >= top && my <= top + height;
    }
    //绘制自己
    virtual void draw() const {};
    virtual ~CHControl() {};

};

//自绘制文本显示框
class TextBox :public CHControl {
public:
    TextBox(std::string name, std::string title, int l, int t, int w, int h) :CHControl(name, title, l, t, w, h) {}

    void draw() const {
        if (title == "")
            return;
        settextcolor(BLACK);
        setbkmode(TRANSPARENT); // 设置矩形背景颜色
        outtextxy(left + 10, top + 10, title.c_str());//！！！！！！
    }
    //virtual void onclick()=0;
};

//自绘制文本输入框
class EditBox :public CHControl {
public:
    EditBox(std::string name, std::string title, int l, int t, int w, int h) :CHControl(name, title, l, t, w, h) {}
    bool focus = false;//是否处于焦点
    bool caretShow = false;//是否显示光标。

    void draw() const {
        const int linewidth = 2;
        setlinestyle(PS_SOLID, linewidth);

        //  主体
        setfillcolor(RGB(255, 255, 255));
        solidrectangle(left, top, left + width, top + height);
        // 左上 = 阴影（凹下去）
        setlinecolor(RGB(100, 100, 100));
        line(left - linewidth, top, left + width, top);     // 上
        line(left, top, left, top + height);     // 左
        //  右下 = 高光
        setlinecolor(RGB(180, 180, 180));
        line(left, top + height, left + width, top + height); // 下
        line(left + width, top, left + width, top + height); // 右

        std::string tmptext = title;
        if (caretShow)
            tmptext += "|";

        if (tmptext == "")
            return;
        settextcolor(BLACK);
        setbkmode(TRANSPARENT); // 设置矩形背景颜色
        outtextxy(left + 10, top + 10, tmptext.c_str());
    }
};



// 自绘制按钮
class Button :public CHControl {
public:
    Button(std::string name, std::string title, int l, int t, int w, int h) :CHControl(name, title, l, t, w, h) {
        int x = 0;
    }
    bool m_clicked = false;           // 状态


    void draw() const {
        const int linewidth = 2;
        setlinestyle(PS_SOLID, linewidth);

        setfillcolor(m_clicked ? BKGCOLOR : RGB(100, 100, 100));
        solidrectangle(left + 3, top + 3, left + width + 3, top + height + 3);

        // 主体
        setfillcolor(RGB(74, 144, 226));
        solidrectangle(left, top, left + width, top + height);
        //  高光边（上、左）
        setlinecolor(m_clicked ? RGB(80, 80, 80) : RGB(255, 255, 255));
        line(left - linewidth, top, left + width, top);     // 上
        line(left, top, left, top + height);     // 左
        // 阴影边（下、右）
        setlinecolor(m_clicked ? RGB(255, 255, 255) : RGB(80, 80, 80));
        line(left, top + height, left + width, top + height); // 下
        line(left + width, top, left + width, top + height); // 右


        if (title == "")
            return;
        settextcolor(BLACK);
        setbkmode(TRANSPARENT); // 设置矩形背景颜色
        outtextxy(left + 10, top + 10, title.c_str());
    }
};

// 自绘制圆形按钮
class CirButton :public Button{
protected:
    double radius;
public:
    CirButton(string name, string title, double l, double t, double rad) :Button(name, title, l, t, rad * 2, rad * 2), radius(rad) {}
    ~CirButton() {}
public:

    void draw() const {
        setfillcolor(m_clicked ? RGB(0, 0, 0) : RGB(74, 144, 226));
        // left 和 top 作为圆心坐标
        solidcircle(left , top , radius);

        if (title == "")
            return;
        settextcolor(WHITE);
        int textX = left - textwidth(title.c_str()) / 2;
        int textY = top - textheight(title.c_str()) / 2;
        outtextxy(textX, textY, title.c_str());
    }

    bool isPointInButton(int mx, int my) const override {
        int dx = mx - left;
        int dy = my - top;
        return dx * dx + dy * dy <= radius * radius;
    }
};



//// 绘制按钮 
//void drawButtons(const std::vector<Button>& buttons) {
//    for (const auto btn : buttons) {
//        btn.draw();
//    }
//}

void drawControls(const std::vector<CHControl*>& controls) {
    setorigin(0, 0);
    for (const auto ctl : controls) {
        ctl->draw();
    }
}


// 绘制函数
// formulaStr: 输入公式，例如 "sin(x)+x*0.1"
// xStart, xEnd: x 范围
// step: 步长
// 返回值: 是否绘制成功
bool drawFunction(const std::string& fuctionStr,
    double xStart, double xEnd, double step) {

    exprtk::symbol_table<double> symbol_table;

    double x;
    symbol_table.add_variable("x", x);

    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);
    exprtk::parser<double> parser;
    if (!parser.compile(fuctionStr, expression)) {
        std::cerr << "公式解析失败!" << std::endl;
        return false;
    }

    setorigin(ScreenWidth / 2, ScreenHeight / 2);
    x = xStart;
    double x1 = xStart;
    double y1 = expression.value();
    x += step;
    while (x <= xEnd) {
        double x2 = x;
        double y2 = expression.value();
        line(x1 * 15, -y1 * 15, x2 * 15, -y2 * 15);//缩放比例！
        x1 = x2;
        y1 = y2;
        x += step;

    }

     // 此处计算绘制
    for (x = xStart; x <= xEnd; x += step) {
        std::cout << x << " " << expression.value() << std::endl;
    }
    setorigin(0, 0);
    return true;
}


//坐标轴绘制
void drawAxes(int scale) {
    setorigin(ScreenWidth / 2, ScreenHeight / 2);
    setlinecolor(BLACK);
    // X 轴
    line(-ScreenWidth / 2, 0, ScreenWidth / 2, 0);
    // Y 轴
    line(0, -ScreenHeight / 2, 0, ScreenHeight / 2);

	// 标刻度
	// X 轴刻度
    for (int i = -ScreenWidth / 2; i <= ScreenWidth / 2; i += scale) {
        if (i == 0)    continue;
		line(i, -5, i, 5); //刻度线
		
    }
	// Y 轴刻度
    for(int i = -ScreenHeight / 2; i <= ScreenHeight / 2; i += scale) {
		if (i == 0)    continue;
        line(-5, i, 5, i); //刻度线
	}

    setorigin(0, 0);
}


int main()
{
    //开始时间
    clock_t start = clock();
    //记录所有control指针，
    std::vector<CHControl*> controls;
    controls.push_back(new TextBox("label1", "输入函数：", 10, 10, 85, 35));
    EditBox* func_editbox = new EditBox{ "func_editbox","sin(x)",100, 10, 800, 35 };
    controls.push_back(func_editbox);

    controls.push_back(new Button("drawfunc", "绘制函数", 910, 10, 85, 35));

    controls.push_back(new TextBox("label2", "起始值：", 10, 50, 85, 35));
    EditBox* startbox = new EditBox{ "startbox","0",100, 50, 80, 35 };
    controls.push_back(startbox);

    controls.push_back(new TextBox("label3", "结束值：", 200, 50, 85, 35));
    EditBox* endbox = new EditBox{ "endbox","10",290, 50, 80, 35 };
    controls.push_back(endbox);

    controls.push_back(new TextBox("label4", "步长：", 380, 50, 85, 35));
    EditBox* stepbox = new EditBox{ "stepbox","0.01",470, 50, 80, 35 };
    controls.push_back(stepbox);

    controls.push_back(new Button("scale", "比例", 50, 500, 85, 35));

    controls.push_back(new Button("clearScreen", "清除绘制", 560, 50, 85, 35));


    initgraph(ScreenWidth, ScreenHeight);	// 创建绘图窗口，大小为 1024x800 像素
    setbkcolor(BKGCOLOR);
    cleardevice();
    drawControls(controls);
	drawAxes();

    //上一次鼠标点击过的control
    CHControl* preclickedCtrl = nullptr;
    //当前点击的ctrl
    CHControl* curclickedCtrl = nullptr;

    while (true) {
        // Esc 退出
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            break;


        std::string clicledButtonName = "";
        // 鼠标点击检测
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            //重置选中的control
            if (msg.uMsg == WM_LBUTTONDOWN || msg.uMsg == WM_LBUTTONUP)
                curclickedCtrl = nullptr;
            for (auto ctrl : controls) {
                if (!ctrl->isPointInButton(msg.x, msg.y))
                    continue;

                //在运行时检查并转换一个基类指针到一个派生类指针，如果成功则说明是Button
                Button* btn = dynamic_cast<Button*>(ctrl);

                //使button有按下的状态
                if (msg.uMsg == WM_LBUTTONDOWN) {
                    if (btn) btn->m_clicked = true;
                    std::cout << ctrl->name << " mousedown" << std::endl;;

                }
                if (msg.uMsg == WM_LBUTTONUP) {
                    if (btn) {
                        btn->m_clicked = false;
                        clicledButtonName = btn->name;
                    }
                    std::cout << ctrl->name << " mouseup" << std::endl;
                    curclickedCtrl = ctrl;
                }
                if (btn) ctrl->draw();

                break;
            }

        }

        //Button按键事件处理
        Button* curbtn = dynamic_cast<Button*>(curclickedCtrl);
        if (curbtn) {
            if (clicledButtonName == "clearScreen")
            {
                cleardevice();
                drawControls(controls);
            }

            if (clicledButtonName == "drawfunc")
            {
                endbox->title;
                stepbox->title;
                if (func_editbox->title != "")
                    try {
                    double start = std::stod(startbox->title); // 会抛异常
                    double end = std::stod(endbox->title);
                    double step = std::stod(stepbox->title);

                    drawFunction(func_editbox->title, start, end, step);
                }
                catch (const std::exception& e) {
                    std::cout << "转换失败: " << e.what() << std::endl;
                }
            }

        }

        //------------输入框处理--------------
        EditBox* editbox = dynamic_cast<EditBox*>(curclickedCtrl);
        //输入框输入处理
        int vk = getPrintableKeyPressed();
        if (vk != 0) {
            if (editbox) {
                bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                char ch = vkToCharPrintable(vk, shift);
                if (ch != 0) editbox->title += ch;
                editbox->draw();
                Sleep(150);  // 防止重复
            }
        }

        //输入框退格处理
        if ((GetAsyncKeyState(VK_BACK) & 0x8000) && editbox) {
            if (!editbox->title.empty()) {
                editbox->title.pop_back();
                editbox->draw();
                Sleep(150);  // 防止重复
            }

        }
        //输入框光标闪烁处理
        if (preclickedCtrl != curclickedCtrl) {
            EditBox* editbox = dynamic_cast<EditBox*>(preclickedCtrl);
            if (editbox != nullptr) {
                editbox->caretShow = false;
                editbox->draw();
            }
            preclickedCtrl = curclickedCtrl;
        }
        clock_t end = clock();
        if ((end - start) > 500 && editbox) {
            editbox->caretShow = !editbox->caretShow;
            editbox->draw();
            start = end;
        }

    }

    for (auto ctl : controls) {
        delete ctl;
    }

    closegraph();
    return 0;
}
