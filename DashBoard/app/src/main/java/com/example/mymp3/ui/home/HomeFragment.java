package com.example.mymp3.ui.home;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.example.mymp3.MainActivity;
import com.example.mymp3.R;
import com.example.mymp3.databinding.FragmentHomeBinding;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Set;

public class HomeFragment extends Fragment {

    private FragmentHomeBinding binding;
    TextView textStatus;
    Button btnParied, btnSearch, btnSend;
    ListView listView;

    static BluetoothAdapter btAdapter;
    private final static int REQUEST_ENABLE_BT = 1;

    Set<BluetoothDevice> pairedDevices;
    ArrayAdapter<String> btArrayAdapter;
    ArrayList<String> deviceAddressArray;


    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        HomeViewModel homeViewModel =
                new ViewModelProvider(this).get(HomeViewModel.class);

        binding = FragmentHomeBinding.inflate(inflater, container, false);
        View root = binding.getRoot();
        root = inflater.inflate(R.layout.fragment_home, container, false);

        // Enable bluetooth
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (!btAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        textStatus = (TextView) root.findViewById(R.id.text_status);
        btnParied = (Button) root.findViewById(R.id.btn_paired);
        btnSearch = (Button) root.findViewById(R.id.btn_search);
        btnSend = (Button) root.findViewById(R.id.btn_send);
        listView = (ListView) root.findViewById(R.id.listview);

        // show paired devices
        btArrayAdapter = new ArrayAdapter<>(getContext(), android.R.layout.simple_list_item_1);
        deviceAddressArray = new ArrayList<>();
        listView.setAdapter(btArrayAdapter);

        return root;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    public void onClickButtonPaired(View view){
        btArrayAdapter.clear();
        if(deviceAddressArray!=null && !deviceAddressArray.isEmpty()){ deviceAddressArray.clear(); }
        pairedDevices = btAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
                btArrayAdapter.add(deviceName);
                deviceAddressArray.add(deviceHardwareAddress);
            }
        }
    }

    public void onClickButtonSearch(View view){
        // Check if the device is already discovering
        if(btAdapter.isDiscovering()){
            btAdapter.cancelDiscovery();
        } else {
            if (btAdapter.isEnabled()) {
                btAdapter.startDiscovery();
                btArrayAdapter.clear();
                if (deviceAddressArray != null && !deviceAddressArray.isEmpty()) {
                    deviceAddressArray.clear();
                }
                IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
                //registerReceiver(receiver, filter);
            } else {
                Toast.makeText(getContext(), "bluetooth not on", Toast.LENGTH_SHORT).show();
            }
        }
    }

    // Create a BroadcastReceiver for ACTION_FOUND.
    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // Discovery has found a device. Get the BluetoothDevice
                // object and its info from the Intent.
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
                btArrayAdapter.add(deviceName);
                deviceAddressArray.add(deviceHardwareAddress);
                btArrayAdapter.notifyDataSetChanged();
            }
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Don't forget to unregister the ACTION_FOUND receiver.
        //unregisterReceiver(receiver);
    }

}