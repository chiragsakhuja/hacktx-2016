package com.utexas.nikhildixit.hacktx_client;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import org.json.JSONObject;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    // Network
    private Socket socket;

    // Sensor
    private SensorManager mSensorManager;
    private Sensor mAccel;

    // Game
    private static final float FPS = 60.0f;
    private static final float TIMESTEP = 1.0f / FPS;
    private float[] last_accel_value;
    private float[] my_paddle_position;
    private float[] my_paddle_velocity;
    private float[] my_paddle_acceleration;


    private static final int SERVER_PORT = 1221;
    private static final String SERVER_NAME = "a-dev.me";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize sensor values
        last_accel_value = new float[3];

        // Setup accelerometer
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mAccel = mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        mSensorManager.registerListener(this, mAccel, SensorManager.SENSOR_DELAY_NORMAL);

        // Setup game data
        my_paddle_position = new float[2];
        my_paddle_velocity = new float[3];
        my_paddle_acceleration = new float[3];

        my_paddle_position[0] = my_paddle_position[1] = 0.5f;
        my_paddle_velocity[0] = my_paddle_velocity[1] = my_paddle_velocity[2] = 0.0f;
        my_paddle_acceleration[0] = my_paddle_acceleration[1] = my_paddle_acceleration[2] = 0.0f;

        // Start game thread
        new Thread(new ClientThread()).start();
    }

    public void onSensorChanged(SensorEvent event) {

        if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
            last_accel_value[0] = event.values[0];
            last_accel_value[1] = event.values[1];
            last_accel_value[2] = event.values[2];
        }
    }

    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    public void onClick(View view) {
        try {
            //EditText et = (EditText) findViewById(R.id.EditText01);
            PrintWriter out = new PrintWriter(new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream())),
                    true);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    class ClientThread implements Runnable {

        @Override
        public void run() {

            try {

                // Setup UDP
                InetAddress serverAddr = InetAddress.getByName(SERVER_NAME);
                DatagramSocket client_socket;

                for (;;) {

                    // Update paddle position
                    my_paddle_position[0] += last_accel_value[1] * 0.03f;
                    my_paddle_position[1] += last_accel_value[0] * 0.03f;

                    // Bounds
                    if (my_paddle_position[0] < 0.0f)
                        my_paddle_position[0] = 0.0f;

                    if (my_paddle_position[0] > 1.0f)
                        my_paddle_position[0] = 1.0f;

                    if (my_paddle_position[1] < 0.0f)
                        my_paddle_position[1] = 0.0f;

                    if (my_paddle_position[1] > 1.0f)
                        my_paddle_position[1] = 1.0f;


                    // Create JSON object
                    JSONObject data = new JSONObject();
                    try {
                        data.put("paddle_id", 0);
                        data.put("paddle_x", my_paddle_position[0]);
                        data.put("paddle_y", my_paddle_position[1]);
                    } catch (Exception e) {
                    }

                    try {

                        // Send paddle info
                        byte[] byte_data = data.toString().getBytes(StandardCharsets.US_ASCII);
                        DatagramPacket send_packet = new DatagramPacket(byte_data, data.toString().length(), serverAddr, SERVER_PORT);
                        client_socket = new DatagramSocket();
                        client_socket.send(send_packet);

                        // Wait for response (NOT NEEDED IN PRODUCTION)
                        //byte[] response = new byte[1024];
                        //DatagramPacket resp_packet = new DatagramPacket(response, response.length, serverAddr, SERVER_PORT);
                        //client_socket.receive(resp_packet);

                    } catch (Exception e) {
                        System.out.println(e.toString());
                    }
                    SystemClock.sleep(1000000);
                }

            } catch (UnknownHostException e1) {
                e1.printStackTrace();
            } catch (IOException e1) {
                e1.printStackTrace();
            }

        }

    }
}


//socket = new Socket(serverAddr, SERVER_PORT);

           /*     out = new PrintWriter(new BufferedWriter(
                        new OutputStreamWriter(socket.getOutputStream())),
                        true);
*/