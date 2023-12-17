#include <math.h>

const short MAX_STEP = 5;
short stepSize = 5;

struct _rgb
{
    short red;
    short green;
    short blue;

    _rgb()
    {
        red = green = blue = 0;
    }

    _rgb(short r, short g, short b)
    {
        red = r;
        green = g;
        blue = b;
    }

    int determine_step(short original, short target, short stepSize)
    {
        int result = original;
        if (original < target)
        {
            result = min(original + stepSize, target);
        }
        else if (original > target)
        {
            result = max(original - stepSize, target);
        }
        return result;
    }

    void fade_step(_rgb target)
    {
        red = determine_step(red, target.red, stepSize);
        green = determine_step(green, target.green, stepSize);
        blue = determine_step(blue, target.blue, stepSize);
    }

    inline bool operator==(const _rgb &rhs)
    {
        return red == rhs.red && green == rhs.green && blue == rhs.blue;
    }
    int total()
    {
        return (int)red + green + blue;
    }

    short calc_step() {
        short count = 0;
        if (red > 0) {
            count++;
        }
        if (green > 0) {
            count++;
        }
        if (blue > 0) {
            count++;
        }
        return MAX_STEP / count;
    }
} rgb;

short numPins = 3;
short pins[3] = {3, 5, 6};

void setup()
{
    for (int i = 0; i < numPins; i++)
    {
        short current_pin = pins[i];
        pinMode(current_pin, OUTPUT);
        analogWrite(current_pin, 0);
    }
    Serial.begin(9600);
}

void serial_log_value(_rgb value)
{
    Serial.print("(");
    Serial.print(value.red);
    Serial.print(", ");
    Serial.print(value.green);
    Serial.print(", ");
    Serial.print(value.blue);
    Serial.print(")");
    Serial.println("");
}

void write_rgb(_rgb value)
{
    int values[3] = {value.red, value.green, value.blue};
    for (int i = 0; i < numPins; i++)
    {
        analogWrite(pins[i], values[i]);
    }
    // Serial.print("  Writing:");
    // serial_log_value(value);
}

const short MAX_BRIGHTNESS = 110;

int values[3] = {0, 0, 0};
int value_to_bump = 0;

_rgb colorPatterns[] = {
    _rgb(255, 0, 0),
    _rgb(200, 0, 255),
    _rgb(255, 255, 255),
};
int collorPatternsNum = sizeof(colorPatterns) / sizeof(_rgb);

_rgb current = _rgb();
short target_index = 0;
_rgb target = colorPatterns[target_index];
_rgb BLACK = _rgb();
int loop_deplay = 60;

void loop()
{
    current.fade_step(target);

    // slight delay so it doesn't rotate color too quicky
    delay(loop_deplay);
    write_rgb(current);

    if (current == target || current.total() > MAX_BRIGHTNESS)
    {
        delay(100);
        Serial.println("Reached target");
        while (current.total() > BLACK.total()) {
            current.fade_step(BLACK);
            delay(loop_deplay);
            write_rgb(current);
        }
        delay(400);
        Serial.println("Faded to black");
        target = colorPatterns[++target_index % collorPatternsNum];
        stepSize = min(target.calc_step(), MAX_STEP);
        Serial.print("New StepSize: ");
        Serial.println(stepSize);
        Serial.print("Now aiming for: ");
        serial_log_value(target);
    }
}
