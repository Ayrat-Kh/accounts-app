package com.airatkhisamiev.accounts

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.material3.Surface
import androidx.compose.ui.Modifier
import com.airatkhisamiev.accounts.ui.AppNavigation
import com.airatkhisamiev.accounts.ui.theme.AccountsTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        val container = (application as AccountsApplication).container
        setContent {
            AccountsTheme {
                Surface(Modifier.fillMaxSize().safeDrawingPadding()) {
                    AppNavigation(container)
                }
            }
        }
    }
}
