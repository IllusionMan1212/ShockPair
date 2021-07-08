package com.illusionware.shockpair

import android.content.*
import android.os.Bundle
import android.os.IBinder
import android.util.Log
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.topjohnwu.superuser.Shell
import com.topjohnwu.superuser.ipc.RootService
import java.io.IOException


class MainActivity : AppCompatActivity() {

    companion object {
        init {
            Shell.enableVerboseLogging = BuildConfig.DEBUG
            Shell.setDefaultBuilder(
                Shell.Builder.create()
                    .setFlags(Shell.FLAG_REDIRECT_STDERR)
            )
        }
    }

    var rootIPC : IRootService? = null
    var daemonTestQueued = false

    var recyclerView : RecyclerView? = null
    var connectDS3Button : Button? = null

    private val AIDLConnection = object : ServiceConnection {
        override fun onServiceConnected(name: ComponentName, service: IBinder) {
            Log.d("whatever", "daemon onServiceConnected")
            rootIPC = IRootService.Stub.asInterface(service)
            if (daemonTestQueued) {
                daemonTestQueued = false
                testDaemon()
            }
        }

        override fun onServiceDisconnected(name: ComponentName) {
            Log.d("whatever", "daemon onServiceDisconnected")
            rootIPC = null
        }
    }

    private fun testDaemon() {
        Log.d("whatever", "Daemon PID: ${rootIPC?.pid}")
        Log.d("whatever", "Daemon UID: ${rootIPC?.uid}")
        Log.d("whatever", rootIPC?.connectToDS3().toString())
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val testDaemonButton = findViewById<Button>(R.id.testDaemonButton)
        testDaemonButton.setOnClickListener {
            if (rootIPC == null) {
                daemonTestQueued = true;
                val intent = Intent(applicationContext, AIDLService::class.java)
                RootService.bind(intent, AIDLConnection)
                return@setOnClickListener
            }
            testDaemon()
        }

        val stopDaemonButton = findViewById<Button>(R.id.stopDaemonButton)
        stopDaemonButton.setOnClickListener {
            val intent = Intent(applicationContext, AIDLService::class.java)
            // we call RootService.stop() if the process is running as daemon
            // we call unbind() if the process is not running as daemon
            //RootService.unbind(AIDLConnection)
            RootService.stop(intent)
        }

        val closeShellButton = findViewById<Button>(R.id.closeShellButton)
        closeShellButton.setOnClickListener {
            try {
                val shell = Shell.getCachedShell()
                shell?.close()
            } catch (e: IOException) {
                Log.e("whatever", "Error when closing shell: ", e)
            }
        }

        // line to ask for root access. check out
        // https://stackoverflow.com/questions/4905743/android-how-to-gain-root-access-in-an-android-application
        // for a full breakdown
        //val suProcess = Runtime.getRuntime().exec("su")

        // init the recyclerview
        recyclerView = findViewById<RecyclerView>(R.id.recycler)
        recyclerView?.layoutManager = LinearLayoutManager(applicationContext)

        connectDS3Button = findViewById<Button>(R.id.connectDS3Button)

        connectDS3Button!!.setOnClickListener {
            Toast.makeText(applicationContext, "Attempting to Connect", Toast.LENGTH_SHORT).show()
        }

    }
}