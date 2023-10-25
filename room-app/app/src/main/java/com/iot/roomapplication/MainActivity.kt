/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

package com.iot.roomapplication

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.roomapplication.R

class MainActivity : AppCompatActivity() {
    val foundDevices = mutableListOf<BluetoothDevice>()
    val pairedDevices = mutableListOf<BluetoothDevice>()

    val foundDevicesAdapter = DeviceAdapter(foundDevices)
    val pairedDevicesAdapter = DeviceAdapter(pairedDevices)

    val deviceFoundBroadcastReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            Log.d("bt", "onReceive")
            val action = intent?.action
            Log.d("receiver", action.toString())
            if (BluetoothDevice.ACTION_FOUND == action) {
                val device = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                if (device != null) {
                    onBluetoothDeviceFound(device)
                }
            }
        }
    }

    fun onBluetoothDeviceFound(device : BluetoothDevice) {
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
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), 2)
        }

        if (device?.name != null)
            foundDevices.add(device)

        runOnUiThread{ foundDevicesAdapter.notifyDataSetChanged() }
    }

    class DeviceAdapter(private val dataSet: MutableList<BluetoothDevice>) : RecyclerView.Adapter<DeviceAdapter.ViewHolder>() {
        class ViewHolder(val listElementView: View) : RecyclerView.ViewHolder(listElementView) {
            val deviceNameText: TextView

            init {
                deviceNameText = listElementView.findViewById(R.id.textView)
            }

            fun bind(bluetoothDevice: BluetoothDevice) {
                try {
                    deviceNameText.text = bluetoothDevice.name
                } catch (e: SecurityException) {
                    Log.d("exception", "security exception in ViewHolder bind")
                }
            }
        }

        // Create new views (invoked by the layout manager)
        override fun onCreateViewHolder(viewGroup: ViewGroup, viewType: Int): ViewHolder {
            // Create a new view, which defines the UI of the list item
            val view = LayoutInflater.from(viewGroup.context)
                .inflate(R.layout.device_item, viewGroup, false)

            return ViewHolder(view)
        }

        // Replace the contents of a view (invoked by the layout manager)
        override fun onBindViewHolder(viewHolder: ViewHolder, position: Int) {
            Log.d("bind view holder", position.toString())
            // Get element from your dataset at this position and replace the
            // contents of the view with that element
            if (dataSet.isNotEmpty()) {
                try {
                    val bluetoothDevice = dataSet[position]
                    viewHolder.bind(bluetoothDevice)
                    viewHolder.listElementView.setOnClickListener{
                        // Handle item click here
                        val context = viewHolder.itemView.context
                        val intent = Intent(context, ControlActivity::class.java)

                        // You can pass data to the new activity if needed
                        intent.putExtra("X_BLUETOOTH_DEVICE_EXTRA", bluetoothDevice)

                        Log.d("click listener", bluetoothDevice.name)

                        context.startActivity(intent)
                    }
                } catch (e : SecurityException) {
                    Log.d("exception", e.toString())
                }
            }
        }

        // Return the size of your dataset (invoked by the layout manager)
        override fun getItemCount() = dataSet.size
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val foundDevicesRecyclerView: RecyclerView = findViewById(R.id.foundRecyclerView)
        val pairedDevicesRecyclerView: RecyclerView = findViewById(R.id.pairedRecyclerView)

        foundDevicesRecyclerView.layoutManager = LinearLayoutManager(this)
        pairedDevicesRecyclerView.layoutManager = LinearLayoutManager(this)

        foundDevicesRecyclerView.adapter = foundDevicesAdapter
        pairedDevicesRecyclerView.adapter = pairedDevicesAdapter

        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE)
        if (bluetoothManager == null) {
            Log.d("bluetooth", "Bluetooth not available: getSystemService(BLUETOOTH_SERVICE) returned null")
            runOnUiThread{ pairedDevicesRecyclerView.adapter?.notifyDataSetChanged() }
            return
        }

        bluetoothManager as BluetoothManager
        val bluetoothAdapter: BluetoothAdapter = bluetoothManager.adapter
        pairedDevices.clear()

        ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH_SCAN), 1)
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
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN, Manifest.permission.BLUETOOTH_CONNECT, Manifest.permission.BLUETOOTH_SCAN), 1)
        }
        pairedDevices.addAll(bluetoothAdapter.bondedDevices)
        runOnUiThread { foundDevicesRecyclerView.adapter?.notifyDataSetChanged() }

        val button = findViewById<Button>(R.id.scanButton)
        button.setOnClickListener {
            Log.d("Button", "button pressed")
            bluetoothAdapter.cancelDiscovery()
            foundDevices.clear()
            runOnUiThread { foundDevicesRecyclerView.adapter?.notifyDataSetChanged() }
            bluetoothAdapter.startDiscovery()
        }

        val filter : IntentFilter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        val receiverFlags = ContextCompat.RECEIVER_EXPORTED
        ContextCompat.registerReceiver(this, deviceFoundBroadcastReceiver, filter, receiverFlags)
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onDestroy() {
        super.onDestroy()
    }
}