package com.pen_input;

import static android.content.ContentValues.TAG;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class UsbConnectedActivity extends AppCompatActivity {
    private static final String ACTION_USB_PERMISSION = "com.pen_input.USB_PERMISSION";
    private UsbManager usbManager;
    private View mainView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        mainView = findViewById(R.id.main);
        ViewCompat.setOnApplyWindowInsetsListener(mainView, (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        requestPermissionForAccessory();
    }

    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    private void requestPermissionForAccessory() {
        UsbAccessory[] usbAccessories = usbManager.getAccessoryList();
        if(usbAccessories != null && usbAccessories.length > 0){
            UsbAccessory usbAccessory = usbAccessories[0];
            PendingIntent permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
            IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
            registerReceiver(usbReceiver, filter);
            usbManager.requestPermission(usbAccessory, permissionIntent);
        }
        else{
            String message = "Could not find any accessory while attempting to request permission to newly connected accessory - this should not happen.";
            openAlertDialog(message);
            Log.d(TAG, message);
        }
    }


    private final BroadcastReceiver usbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    UsbAccessory usbAccessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        UsbStreamService.streamTouchInputToUsb(usbManager, usbAccessory, mainView);
                    } else {
                        String message = "permission denied for accessory " + usbAccessory;
                        openAlertDialog(message);
                        Log.d(TAG, message);
                    }
                }
            }
        }
    };

    private void openAlertDialog(String message){
        runOnUiThread(() -> {
            new AlertDialog.Builder(mainView.getContext()).setTitle("Warning")
                    .setMessage(message)
                    .show();
        });
    }
}
