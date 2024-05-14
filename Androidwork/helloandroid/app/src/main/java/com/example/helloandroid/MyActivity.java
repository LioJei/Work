package com.example.helloandroid;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.content.pm.PackageManager;
import android.telephony.SmsManager;
import android.widget.EditText;

import java.util.ArrayList;
import java.util.List;

public class MyActivity extends Activity {
    /*
     * Called when the activity is first created.
     */
    static String[] permissions = {
            "android.permission.SEND_SMS",
            "android.permission.READ_PHONE_STATE"
    };
    static String[] menuStr = {
            "dspg", "lhsr", "tdnr", "szrp", "mzjc", "lzjd", "sltds", "fqcd"
    };
    private String sendStr = "已点: ";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        List<String> list = new ArrayList<>();
        list.clear();
        for (int i = 0; i < permissions.length; i++) {
            if (ContextCompat.checkSelfPermission(this, permissions[i]) != PackageManager.PERMISSION_GRANTED) {
                list.add(permissions[i]);
            }
        }
        if (list.isEmpty()) {
            //fun();
        } else {
            String[] needP = list.toArray(new String[list.size()]);
            ActivityCompat.requestPermissions(this, needP, 100);
        }
        EditText sendText = (EditText) findViewById(R.id.menuText);
        EditText addText = (EditText) findViewById(R.id.addText);
        for (String name : menuStr) {
            int resourceId = getResources().getIdentifier(name, "id", getPackageName());
//            String val = getString(resourceId);
            Button button = (Button) findViewById(resourceId);
            button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Button btn = (Button) v;
                    String str = btn.getText().toString();
                    sendStr += str + " ";
                    sendText.setText(sendStr);
                }
            });
        }
        //得到按钮实例
        Button sendBtn = (Button) findViewById(R.id.sendBtn);
        //设置监听按钮点击事件
        sendBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String content = sendStr.trim();
                String phone = "13684430603";
                SmsManager  smsManager = SmsManager.getDefault();
                List<String> list=smsManager.divideMessage(content);
                for(String sms:list){
                    smsManager.sendTextMessage(phone,null,sms,null,null);
                }
                Toast.makeText(MyActivity.this, "发送成功", Toast.LENGTH_SHORT);
            }
        });
        Button addBtn = (Button) findViewById(R.id.addBtn);
        addBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendStr += addText.getText().toString() + " ";
                sendText.setText(sendStr);
            }
        });
        Button clearBtn = (Button) findViewById(R.id.clearBtn);
        clearBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendStr ="已点: ";
                sendText.setText(sendStr);
            }
        });
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch(requestCode){
            case 1:
                for(int i=0;i<grantResults.length;i++){
                    if(grantResults[i]!=PackageManager.PERMISSION_GRANTED){
                        Toast.makeText(this, "权限不足，功能无法全部实现!", Toast.LENGTH_SHORT).show();
                        finish();
                    }
                }
//                fun();
        }
    }
}
