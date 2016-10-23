package com.utexas.nikhildixit.hacktx_client;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.StrictMode;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Base64;

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
import android.util.Base64;
import android.view.View;
import android.widget.EditText;

import org.json.JSONObject;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    // Network
    private Socket socket;

    // Sensor
    private SensorManager mSensorManager;
    private Sensor mAccel;
    private Sensor mMag;

    // Game
    private static final float FPS = 60.0f;
    private static final float TIMESTEP = 1.0f / FPS;
    private static final int HISTORY_LENGTH = 2;
    private float[][] last_accel_values;
    private float[] my_paddle_position;
    private float[] my_paddle_velocity;
    private int my_id;

    private static final int SERVER_PORT = 1221;
    private static final String SERVER_NAME = "a-dev.me";

    float[] accel;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize sensor values
        last_accel_values = new float[HISTORY_LENGTH][3];
        accel = new float[3];

        // Setup accelerometer
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mAccel = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mMag = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mSensorManager.registerListener(this, mAccel, SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mMag, SensorManager.SENSOR_DELAY_NORMAL);

        // Setup game data
        my_paddle_position = new float[2]; // Keep previous HISTORY_LENGTH paddle positions
        my_paddle_velocity = new float[2];

        /*
        for (int i = 0; i < HISTORY_LENGTH; i++)
            my_paddle_position[i][0] = my_paddle_position[i][1] = 0.5f;
        my_paddle_velocity[0] = my_paddle_velocity[1] = 0.0f;
*/

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        // Start game thread
        new Thread(new ClientThread()).start();
    }

    float[] mGravity;
    float[] mGeomagnetic;

    float azimuth, roll, pitch;
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
            mGravity = event.values;
        if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
            mGeomagnetic = event.values;
        if (mGravity != null && mGeomagnetic != null) {
            float R[] = new float[9];
            float I[] = new float[9];
            boolean success = SensorManager.getRotationMatrix(R, I, mGravity, mGeomagnetic);
            if (success) {
                float orientation[] = new float[3];
                SensorManager.getOrientation(R, orientation);
                azimuth = orientation[0]; // orientation contains: azimut, pitch and roll
                pitch = orientation[1]; // orientation contains: azimut, pitch and roll
                roll = orientation[2]; // orientation contains: azimut, pitch and roll
            }
        }
    }

    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    public void onClick(View view) {
        try {

            // First we need to poll the server to get our ID
            JSONObject data = new JSONObject();
            data.put("req", "start");

            // Setup UDP
            InetAddress serverAddr = InetAddress.getByName(SERVER_NAME);
            DatagramSocket client_socket;

            // Send paddle info
            byte[] byte_data = data.toString().getBytes();
            DatagramPacket send_packet = new DatagramPacket(byte_data, data.toString().length(), serverAddr, SERVER_PORT);
            client_socket = new DatagramSocket();
            client_socket.send(send_packet);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    float rangeMap(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    class ClientThread implements Runnable {

        // Game loop
        @Override
        public void run() {

            // First we need to poll the server to get our ID
            JSONObject data = new JSONObject();
            try {
                data.put("req", "connect");
                data.put("dev", "phone");

            } catch (Exception e) {
            }

            // Send paddle info to server
            try {

                // Setup UDP
                InetAddress serverAddr = InetAddress.getByName(SERVER_NAME);
                DatagramSocket client_socket;

                // Send paddle info
                byte[] byte_data = data.toString().getBytes();
                DatagramPacket send_packet = new DatagramPacket(byte_data, data.toString().length(), serverAddr, SERVER_PORT);
                client_socket = new DatagramSocket();
                client_socket.send(send_packet);

                // I LOL @ efficiency
                byte[] response = new byte[1024];
                DatagramPacket resp_packet = new DatagramPacket(response, response.length, serverAddr, SERVER_PORT);
                client_socket.receive(resp_packet);
                byte[] new_data = new byte[resp_packet.getLength()];
                System.arraycopy(resp_packet.getData(), resp_packet.getOffset(), new_data, 0, resp_packet.getLength());

                //System.out.println(new String(new_data));
                JSONObject response_json = new JSONObject(new String(new_data));
                my_id = response_json.getInt("id");

                //System.out.println("my id:" + my_id);

            } catch (Exception e) {
                System.out.println(e.toString());
            }

            try {

                InetAddress serverAddr = InetAddress.getByName(SERVER_NAME);
                DatagramSocket client_socket;
                client_socket = new DatagramSocket();

                // Main game loop
                for (;;) {


                  /*  float[] final_acceleration = new float[2];
                    for (int i = 0; i < HISTORY_LENGTH; i++) {
                        final_acceleration[0] += last_accel_values[i][0];
                        final_acceleration[1] += last_accel_values[i][1];
                    }

                    final_acceleration[0] /= (float) HISTORY_LENGTH;
                    final_acceleration[1] /= (float) HISTORY_LENGTH;

                    float accel_limit = 0.1f;
                    if (final_acceleration[0] > accel_limit)
                        final_acceleration[0] = accel_limit;
                    if (final_acceleration[1] > accel_limit)
                        final_acceleration[1] = accel_limit;
                    if (final_acceleration[0] < -accel_limit)
                        final_acceleration[0] = -accel_limit;
                    if (final_acceleration[1] < -accel_limit)
                        final_acceleration[1] = -accel_limit;


                    System.out.println(final_acceleration[0] + "\t" + final_acceleration[1]);
                    */
                  /*
                    float[] final_velocity = new float[2];
                    for (int i = 0; i < HISTORY_LENGTH - 1; i++) {
                        //my_paddle_velocity[i][0] = last_accel_values[i][0] - last_accel_values[i+1][0];
                        //my_paddle_velocity[i][1] = last_accel_values[i][1] - last_accel_values[i+1][1];
                        final_velocity[0] += my_paddle_velocity[i][0];
                        final_velocity[1] += my_paddle_velocity[i][1];
                    }

                    final_velocity[0] /= (float) HISTORY_LENGTH;
                    final_velocity[1] /= (float) HISTORY_LENGTH;
*/
                    /*
                    if (Math.abs(final_acceleration[0]) > 0.005)
                        my_paddle_velocity[0] += final_acceleration[0] * 0.1f;
                    else
                        my_paddle_velocity[0] = 0.0f;
                    if (Math.abs(final_acceleration[1]) > 0.005)
                        my_paddle_velocity[1] += final_acceleration[1] * 0.1f;
                    else
                        my_paddle_velocity[1] = 0.0f;

                    float velocity_limit = 0.01f;
                    if (my_paddle_velocity[0] > velocity_limit)
                        my_paddle_velocity[0] = velocity_limit;
                    if (my_paddle_velocity[1] > velocity_limit)
                        my_paddle_velocity[1] = velocity_limit;
                    if (my_paddle_velocity[0] < -velocity_limit)
                        my_paddle_velocity[0] = -velocity_limit;
                    if (my_paddle_velocity[1] < -velocity_limit)
                        my_paddle_velocity[1] = -velocity_limit;

                    my_paddle_position[0] += last_accel_values[0][1] * 0.01;
                    my_paddle_position[1] += last_accel_values[0][0] * 0.01;
*/
                    float limiter = 25.0f;
                    float azimuth_deg = (float)Math.toDegrees(azimuth);
                    if (azimuth_deg < -limiter)
                        azimuth_deg = -limiter;
                    if (azimuth_deg > limiter)
                        azimuth_deg = limiter;

                    float roll_deg = (float)Math.toDegrees(roll);
                    if (roll_deg < 70-limiter)
                        roll_deg = 70-limiter;
                    if (roll_deg > 70+limiter)
                        roll_deg = 70+limiter;

                    my_paddle_velocity[0] = rangeMap(azimuth_deg, -limiter, limiter, -0.07f, 0.07f);
                    my_paddle_position[0] -= my_paddle_velocity[0];
                    my_paddle_velocity[1] = rangeMap(roll_deg, 70-limiter, 70+limiter, -0.07f, 0.07f);
                    my_paddle_position[1] -= my_paddle_velocity[1];

                    /*
                    if (last_accel_values[0][0] * last_accel_values[1][0] < 0.0)
                        if (Math.abs(last_accel_values[0][0] - last_accel_values[1][0]) > 0.01)
                            System.out.println("GG" + (Math.abs(last_accel_values[0][0]) - Math.abs(last_accel_values[1][0])));

                  /*  if (last_accel_values[0][0] * last_accel_values[1][0] < 0.0)
                        if (Math.abs(last_accel_values[0][0] - last_accel_values[1][0]) > 0.05)
                            System.out.println("Swapped X: " + (last_accel_values[0][0] - last_accel_values[1][0]));
*/
                    // Bounds
                    if (my_paddle_position[0] < -1.0f)
                        my_paddle_position[0] = -1.0f;

                    if (my_paddle_position[0] > 1.0f)
                        my_paddle_position[0] = 1.0f;

                    if (my_paddle_position[1] < -1.0f)
                        my_paddle_position[1] = -1.0f;

                    if (my_paddle_position[1] > 1.0f)
                        my_paddle_position[1] = 1.0f;


                    /*
                    // Update paddle positions
                    for (int i = HISTORY_LENGTH - 1; i > 0; i--) {
                        my_paddle_position[i][0] = my_paddle_position[i - 1][0];
                        my_paddle_position[i][1] = my_paddle_position[i - 1][1];
                    }

                    my_paddle_position[0][0] += last_accel_value[1] * 0.03f;
                    my_paddle_position[0][1] += -last_accel_value[0] * 0.03f;

                    // Bounds
                    if (my_paddle_position[0][0] < -1.0f)
                        my_paddle_position[0][0] = -1.0f;

                    if (my_paddle_position[0][0] > 1.0f)
                        my_paddle_position[0][0] = 1.0f;

                    if (my_paddle_position[0][1] < -1.0f)
                        my_paddle_position[0][1] = -1.0f;

                    if (my_paddle_position[0][1] > 1.0f)
                        my_paddle_position[0][1] = 1.0f;

                    // Generate velocity
                    float[][] position_deltas = new float[HISTORY_LENGTH - 1][2];
                    for (int i = 0; i < HISTORY_LENGTH - 1; i++) {
                        position_deltas[i][0] = my_paddle_position[i][0] - my_paddle_position[i+1][0];
                        position_deltas[i][1] = my_paddle_position[i][1] - my_paddle_position[i+1][1];
                    }

                    for (int i = 0; i < HISTORY_LENGTH - 1; i++) {
                        my_paddle_velocity[0] += position_deltas[i][0];
                        my_paddle_velocity[1] += position_deltas[i][1];
                    }

                    my_paddle_velocity[0] /= (float) HISTORY_LENGTH;
                    my_paddle_velocity[1] /= (float) HISTORY_LENGTH;
*/
                    //System.out.println(my_paddle_velocity[0] + ", " + my_paddle_velocity[1]);

                    // Create JSON object
                    data = new JSONObject();
                    try {
                        data.put("req", "update");
                        data.put("dev", "phone");
                        data.put("id", my_id);
                        data.put("px", my_paddle_position[0]);
                        data.put("py", my_paddle_position[1]);
                        data.put("vx", my_paddle_velocity[0]);
                        data.put("vy", my_paddle_velocity[1]);
                    } catch (Exception e) {
                    }

                    // Send paddle info to server
//                    try {
                        // Setup UDP

                        // Send paddle info
                        byte[] byte_data = data.toString().getBytes();
                        DatagramPacket send_packet = new DatagramPacket(byte_data, data.toString().length(), serverAddr, SERVER_PORT);

                        client_socket.send(send_packet);

                        // Wait for response (NOT NEEDED IN PRODUCTION)
                        //byte[] response = new byte[1024];
                        //DatagramPacket resp_packet = new DatagramPacket(response, response.length, serverAddr, SERVER_PORT);
                        //client_socket.receive(resp_packet);

  //                  } catch (Exception e) {
    //                    System.out.println(e.toString());
      //              }

                    SystemClock.sleep(16);
                }

            } catch (Exception e1) {
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