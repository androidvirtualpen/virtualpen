package com.pen_input;

import static android.view.MotionEvent.AXIS_DISTANCE;
import static android.view.MotionEvent.AXIS_TILT;
import static android.view.MotionEvent.TOOL_TYPE_STYLUS;

import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Locale;

public class TouchListener {
    private final FileOutputStream fileOutputStream;

    public TouchListener(FileOutputStream fileOutputStream) {
        this.fileOutputStream = fileOutputStream;
    }

    public View.OnTouchListener handleTouch = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if(TOOL_TYPE_STYLUS != event.getToolType(0) &&
               MotionEvent.TOOL_TYPE_ERASER != event.getToolType(0)){
               return false;
            }
            int x = (int) event.getX();
            int y = (int) event.getY();
            String inputString = String.format(Locale.US, "%s,%s,%s,%s,%.9f,",
                    event.getToolType(0),
                    event.getAction(),
                    x,
                    y,
                    event.getPressure());
            try {
                fileOutputStream.write(inputString.getBytes(StandardCharsets.UTF_8));
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    Log.i("TAG", "touched down");
                    break;
                case MotionEvent.ACTION_MOVE:
                    Log.i("TAG", "moving: (" + x + ", " + y + ")");
                    break;
                case MotionEvent.ACTION_UP:
                    Log.i("TAG", "touched up");
                    break;
            }

            return true;
        }
    };
}
