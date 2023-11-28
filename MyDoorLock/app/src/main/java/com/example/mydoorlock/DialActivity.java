package com.example.mydoorlock;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.util.Arrays;

public class DialActivity extends AppCompatActivity {

    ConnectedThread connectedThread = MainActivity.connectedThread;
    TextView enteredPwd, statusText;
    char[] buf = "----".toCharArray();
    String outStr = "";
    String pwd;
    boolean isOpen;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dial);

        statusText = findViewById(R.id.text_status);
        enteredPwd = findViewById(R.id.text_pwd);

        statusText.setText("status");

        Button[] btnArr = {
                findViewById(R.id.btn_num0),
                findViewById(R.id.btn_num1),
                findViewById(R.id.btn_num2),
                findViewById(R.id.btn_num3),
                findViewById(R.id.btn_num4),
                findViewById(R.id.btn_num5),
                findViewById(R.id.btn_num6),
                findViewById(R.id.btn_num7),
                findViewById(R.id.btn_num8),
                findViewById(R.id.btn_num9),
                findViewById(R.id.btn_star),
                findViewById(R.id.btn_hash),
                findViewById(R.id.btn_verify),
                findViewById(R.id.btn_set)
        };
    }

    public void onClickNum0(View view) {
        if(connectedThread != null) {
            setBuf('0');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum1(View view) {
        if(connectedThread != null) {
            setBuf('1');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum2(View view) {
        if(connectedThread != null) {
            setBuf('2');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum3(View view) {
        if(connectedThread != null) {
            setBuf('3');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum4(View view) {
        if(connectedThread != null) {
            setBuf('4');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum5(View view) {
        if(connectedThread != null) {
            setBuf('5');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum6(View view) {
        if(connectedThread != null) {
            setBuf('6');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum7(View view) {
        if(connectedThread != null) {
            setBuf('7');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum8(View view) {
        if(connectedThread != null) {
            setBuf('8');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickNum9(View view) {
        if(connectedThread != null) {
            setBuf('9');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickStar(View view) {
        if(connectedThread != null) {
            setBuf('*');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickHash(View view) {
        if(connectedThread != null) {
            setBuf('#');
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickVerify(View view) {
        if(connectedThread != null) {
            if(outStr.length() < 4 || outStr.contains("-")){
                Toast.makeText(getApplicationContext(), "Password is too short!", Toast.LENGTH_SHORT).show();
            } else {
                connectedThread.write("p");
                connectedThread.write(outStr);
                if(outStr.equals(pwd)){
                    if(connectedThread.isOpen()){
                        statusText.setText("Door Closed");
                    } else {
                        statusText.setText("Door Opened");
                    }
                } else {
                    statusText.setText("Intruder Alert!");
                }
            }
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    public void onClickSet(View view) {
        if(connectedThread != null) {
            if(outStr.length() < 4 || outStr.contains("-")){
                Toast.makeText(getApplicationContext(), "Password is too short!", Toast.LENGTH_SHORT).show();
            } else {
                connectedThread.write("s");
                connectedThread.write(outStr);
                pwd = outStr;
                statusText.setText("Door Closed");
            }
        } else {
            Toast.makeText(getApplicationContext(), "need a connection", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        finish();
    }

    public void setBuf(char c) {
        outStr = "";
        for(int i = 3; i > 0; i--){
            buf[i] = buf[i-1];
        }
        buf[0] = c;
        for(int i = 3; i >= 0; i--) {
            outStr += buf[i];
        }
        enteredPwd.setText(outStr);
    }
}
