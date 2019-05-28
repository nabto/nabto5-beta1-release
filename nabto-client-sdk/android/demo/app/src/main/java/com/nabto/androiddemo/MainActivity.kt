package com.nabto.androiddemo


import android.os.Bundle
import android.support.design.widget.Snackbar
import android.support.v7.app.AppCompatActivity;
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import com.nabto.jni.*

import kotlinx.android.synthetic.main.activity_main.*

class LoggerImpl : Logger() {
    public override fun log(m: LogMessage) {
        val msg: String = m.getMessage();
        Log.v("nabto", msg);
    }
}

class Connect {
    val privateKey = """
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIHhv0V+l/DleigTc2uaOfD6cmSmES2RKo1ZBUbZ9gaP1oAoGCCqGSM49
AwEHoUQDQgAE8D4RsB43cGLEYTO8W7kv9uf8eHlpMKL0nk+r0izLyjkoc4MGqr+K
LhCpOXLvy4ExvuXF0NQQ8XnEP3WJ7dY7PQ==
-----END EC PRIVATE KEY-----
    """.trimIndent();
    fun doConnect(): String {
        try {
            val loggerImpl = LoggerImpl();
            val context = Context.create();
            context.setLogger(loggerImpl);
            context.setLogLevel("trace");
            val connection = context.createConnection();
            connection.setProductId("pr-zieg9fvm");
            connection.setDeviceId("de-wiqj7dce");
            connection.setServerKey("sk-998946ba1fc2c64484904eae548f3a3d");
            connection.setServerUrl("https://a.clients.dev.nabto.net");
            connection.setPrivateKey(privateKey);
            connection.connect().waitForResult();
            val stream = connection.createStream();
            stream.open(424242).waitForResult();

            stream.write(BufferRaw(byteArrayOf(42,43,44,45)));
            val result: Buffer = stream.readAll(4).waitForResult();
            val sb = StringBuilder();
            for (i in result.data()) {
                sb.append(i.toString());
            }
            stream.close().waitForResult();
            connection.close().waitForResult();
            return sb.toString();
        } catch (e: Throwable) {
            return "not connected " + e.message;
        }
    }

}
class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setSupportActionBar(toolbar)

        fab.setOnClickListener { view ->
            val connect = Connect();
            val result = connect.doConnect();
            Snackbar.make(view, result, Snackbar.LENGTH_LONG)
                .setAction("Action", null).show()
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            R.id.action_settings -> true
            else -> super.onOptionsItemSelected(item)
        }
    }
}
