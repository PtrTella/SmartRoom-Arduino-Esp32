/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

package com.iot.roomapplication

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.graphics.Color
import android.util.Log
import android.widget.Button
import android.widget.SeekBar
import android.widget.ToggleButton
import java.io.IOException
import java.util.UUID
import java.util.function.Consumer
import androidx.core.app.ActivityCompat


@SuppressLint("MissingPermission")
class BluetoothClientConnectionThread(
    device: BluetoothDevice,
    private val btAdapter: BluetoothAdapter,
    private val context: ControlActivity?,
    private var lightToggle: ToggleButton?,
    private var rollerBar: SeekBar?,
    private val handler: Consumer<BluetoothSocket?>
) :
    Thread() {
    private val socket: BluetoothSocket?
    init {
        // Use a temporary object that is later assigned to socket
        // because socket is final.
        var tmp: BluetoothSocket? = null
        try {
            // Get a BluetoothSocket to connect with the given BluetoothDevice.
            // MY_UUID is the app's UUID string, also used in the server code.
            tmp = device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
        } catch (e: IOException) {
            Log.e("RoomApplication", "Socket's create() method failed", e)
        }
        socket = tmp
    }

    override fun run() {
        // Cancel discovery because it otherwise slows down the connection.
        btAdapter.cancelDiscovery()
        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            if (socket == null) {
                Log.d("RoomApplication", "socket is null")
            }
            socket!!.connect()
        } catch (connectException: IOException) {
            Log.e("RoomApplication", "unable to connect")
            Log.e("RoomApplication", connectException.toString())
            // Unable to connect; close the socket and return.
            try {
                socket!!.close()
            } catch (closeException: IOException) {
                Log.e("RoomApplication", "Could not close the client socket", closeException)
            }
            return
        }
        // The connection attempt succeeded. Perform work associated with
        // the connection in a separate thread.
        handler.accept(socket)

        val inputStream = socket.inputStream
        var dataReceived = ""
        while (true) {
            while (inputStream.available() > 0) {
                dataReceived = dataReceived.plus(inputStream.read().toChar())
            }
//            Log.d("BTData", dataReceived)
            dataReceived.split("\r\n").forEach {
                if (it.matches(Regex("light:on\r?\n?"))) {
//                    Log.d("Actions", "set button: true")
                    context?.setLightToggleButton(true)
                } else if (it.matches(Regex("light:off\r?\n?"))) {
//                    Log.d("Actions", "set button: false")
                    context?.setLightToggleButton(false)
                } else if (it.matches(Regex("roller_blind:\\d{1,3}\r?\n?"))) {
                    val currentRollerBlindSetting = it.split(":")[1].toInt()
//                    Log.d("Actions", "set slider: $currentRollerBlindSetting")
                    context?.setRollerBlindSeekBar(currentRollerBlindSetting)
                }
            }
//            Log.d("DataReceived", "EMPTY")
            dataReceived = ""
            sleep(100)
        }
    }

    // Closes the client socket and causes the thread to finish.
    fun cancel() {
        try {
            socket!!.close()
        } catch (e: IOException) {
            Log.e("RoomApplication", "Could not close the client socket", e)
        }
    }
}