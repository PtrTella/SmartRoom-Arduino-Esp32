/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

package com.iot.roomapplication

import android.Manifest
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.SeekBar
import android.widget.Switch
import android.widget.TextView
import android.widget.ToggleButton
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.SwitchCompat
import androidx.core.app.ActivityCompat
import com.example.roomapplication.R
import java.io.IOException
import java.io.OutputStream
import java.nio.charset.StandardCharsets

class ControlActivity : AppCompatActivity() {
    private var bluetoothOutputStream: OutputStream? = null
    private var lightToggle: ToggleButton? = null
    private var rollerBar: SeekBar? = null
    private var connectionThread: BluetoothClientConnectionThread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_control)
        initUI()
    }

    private fun initUI() {
        lightToggle = findViewById<ToggleButton>(R.id.lightToggle)
        lightToggle?.isEnabled = false
        lightToggle?.setOnClickListener { v : View? ->
            if (lightToggle?.isChecked == true) {
                lightToggle?.setBackgroundResource(R.drawable.toggle_button_on_bg)
            }
            if (lightToggle?.isChecked == false) {
                lightToggle?.setBackgroundResource(R.drawable.toggle_button_off_bg)
            }
            sendLightMessage()
        }
        rollerBar = findViewById(R.id.rollerBar)
        rollerBar?.isEnabled = false
        rollerBar?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                Thread {
                    try {
                        val value = seekBar!!.progress
                        val message = "set_roller_blind:${value.toString()}\n"
                        Log.e("RoomApplication", message)
                        bluetoothOutputStream!!.write(message.toByteArray(StandardCharsets.UTF_8))
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }.start()
            }
        })
    }

    private fun sendLightMessage() {
        Thread {
            try {
                val ledStateString : String = if (lightToggle?.isChecked ?: false) "on" else "off"
                val message = "set_light:$ledStateString\n"
                Log.e("RoomApplication", message)
                bluetoothOutputStream!!.write(message.toByteArray(StandardCharsets.UTF_8))
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }.start()
    }

    public fun setLightToggleButton(checked: Boolean) {
        runOnUiThread {
            lightToggle?.isChecked = checked
            if (checked) {
                lightToggle?.setBackgroundResource(R.drawable.toggle_button_on_bg)
            }
            if (!checked) {
                lightToggle?.setBackgroundResource(R.drawable.toggle_button_off_bg)
            }
        }
    }

    public fun setRollerBlindSeekBar(position: Int) {
        runOnUiThread {
            rollerBar?.progress = position
        }
    }

    override fun onStart() {
        super.onStart()
        var bluetoothDevice: BluetoothDevice? = null
        if (intent.hasExtra("X_BLUETOOTH_DEVICE_EXTRA")) {
            bluetoothDevice =
                intent.getParcelableExtra<BluetoothDevice>("X_BLUETOOTH_DEVICE_EXTRA")

            if (ActivityCompat.checkSelfPermission(
                    this,
                    Manifest.permission.BLUETOOTH_CONNECT
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                Log.d("control activity", "missing permissions")
                return
            }
            val deviceName = bluetoothDevice?.name ?: "Unknown Device"
            val deviceAddress = bluetoothDevice?.address ?: "No Address Available"

            this.findViewById<TextView>(R.id.deviceName).text = deviceName
        }
        if (bluetoothDevice == null) {
            return
        }
        val btAdapter = getSystemService(BluetoothManager::class.java).adapter

        Log.i("RoomApplication", "Connecting to " + bluetoothDevice.name)
        connectionThread = BluetoothClientConnectionThread(bluetoothDevice, btAdapter, this, lightToggle, rollerBar) {
                socket: BluetoothSocket? -> this.manageConnectedSocket(socket!!)
        }
        connectionThread!!.start()
    }

    private fun manageConnectedSocket(socket: BluetoothSocket) {
        try {
            bluetoothOutputStream = socket.outputStream
            Log.i("RoomApplication", "Connection successful!")
        } catch (e: IOException) {
            Log.e("RoomApplication", "Error occurred when creating output stream", e)
        }
//        bluetoothOutputStream?.write("ciao!".toByteArray());
        Log.d("RoomApplication", lightToggle.toString())
        Log.d("RoomApplication", rollerBar.toString())
        runOnUiThread {
            lightToggle?.isEnabled = true
//            if (ledState)
//                lightToggle?.setBackgroundColor(Color.rgb(35, 206, 107))
//            else
//                lightToggle?.setBackgroundColor(Color.rgb(237, 37, 78))
            rollerBar?.isEnabled = true
        }
    }
}