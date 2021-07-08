package com.illusionware.shockpair

import android.content.Intent
import android.os.IBinder
import android.os.Process
import android.util.Log
import com.topjohnwu.superuser.ipc.RootService

// Demonstrate RootService using AIDL (daemon mode)
class AIDLService : RootService() {
    companion object {
        init {
            // Only load the library when this class is loaded in a root process.
            // The classloader will load this class (and call this static block) in the non-root
            // process because we accessed it when constructing the Intent to send.
            // Add this check so we don't unnecessarily load native code that'll never be used.
            if (Process.myUid() == 0) System.loadLibrary("shockpairdriver-lib")
        }
    }

    // Demonstrate we can also run native code via JNI with RootServices
    external fun nativeGetUid(): Int
    external fun nativeConnectToDS3(): Int

    internal inner class TestIPC : IRootService.Stub() {
        override fun getPid(): Int {
            return Process.myPid()
        }

        override fun getUid(): Int {
            return nativeGetUid()
        }

        override fun connectToDS3(): Int {
            return nativeConnectToDS3()
        }
    }

    override fun onRebind(intent: Intent) {
        // This callback will be called when we are reusing a previously started root process
        Log.d("whatever", "AIDLService: onRebind, daemon process reused")
    }

    override fun onBind(intent: Intent): IBinder {
        Log.d("whatever", "AIDLService: onBind")
        return TestIPC()
    }

    override fun onUnbind(intent: Intent): Boolean {
        Log.d("whatever", "AIDLService: onUnbind, client process unbound")
        // We return true here to tell libsu that we want this service to run as a daemon
        // we return false if we don't want it to run as daemon
        return true
    }
}
