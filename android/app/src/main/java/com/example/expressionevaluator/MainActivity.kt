package com.example.expressionevaluator

import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.widget.Button
import android.widget.LinearLayout
import androidx.appcompat.app.AppCompatActivity
import com.example.expressionevaluator.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private val history = StringBuilder()

    companion object {
        init {
            System.loadLibrary("expression_evaluator")
        }
    }

    // Declare native methods
    private external fun initEvaluator()
    private external fun setAngleUnit(mode: Int)
    private external fun getAngleUnit(): Int
    private external fun evaluateExpression(expression: String): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Initialize evaluator
        initEvaluator()

        // Sync angle mode buttons
        val currentAngleMode = getAngleUnit()
        if (currentAngleMode == 1) {
            binding.toggleGroup.check(R.id.btn_deg)
        } else {
            binding.toggleGroup.check(R.id.btn_rad)
        }

        // Setup scroll for terminal
        binding.terminalText.movementMethod = ScrollingMovementMethod()

        // Setup events
        binding.btnRad.setOnClickListener {
            setAngleUnit(0)
            appendToTerminal(">> rad\nModo angular cambiado a RADIANES.\n")
        }

        binding.btnDeg.setOnClickListener {
            setAngleUnit(1)
            appendToTerminal(">> deg\nModo angular cambiado a GRADOS.\n")
        }

        binding.btnClearHistory.setOnClickListener {
            history.setLength(0)
            binding.terminalText.text = "=== Terminal de Evaluador ===\n"
        }

        binding.btnEval.setOnClickListener {
            val expr = binding.edtExpression.text.toString().trim()
            if (expr.isNotEmpty()) {
                evaluateAndShow(expr)
                binding.edtExpression.text.clear()
            }
        }

        // Setup quick helper buttons
        setupQuickButtons()
    }

    private fun evaluateAndShow(expr: String) {
        val output = evaluateExpression(expr)
        val resultText = if (output.isEmpty()) "Error desconocido\n" else output
        appendToTerminal(">> $expr\n$resultText")
    }

    private fun appendToTerminal(text: String) {
        history.append(text)
        binding.terminalText.text = history.toString()
        // Auto scroll to bottom
        binding.terminalScroll.post {
            binding.terminalScroll.fullScroll(android.view.View.FOCUS_DOWN)
        }
    }

    private fun setupQuickButtons() {
        val buttons = listOf(
            "pi", "e", "phi", "i", 
            "+", "-", "*", "/", "^", "%", "(", ")",
            "sin(", "cos(", "tan(", "asin(", "acos(", "atan(", 
            "ln(", "log(", "sqrt(", "abs(", "list", "funct"
        )
        
        val container = binding.quickButtonsContainer
        val layoutParams = LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT
        ).apply {
            setMargins(8, 4, 8, 4)
        }

        for (btnText in buttons) {
            val btn = Button(this, null, com.google.android.material.R.style.Widget_MaterialComponents_Button_OutlinedButton).apply {
                text = btnText
                isAllCaps = false
                textSize = 12f
                this.layoutParams = layoutParams
                setOnClickListener {
                    val currentText = binding.edtExpression.text.toString()
                    val cursorPosition = binding.edtExpression.selectionStart
                    val newText = StringBuilder(currentText).insert(cursorPosition, btnText).toString()
                    binding.edtExpression.setText(newText)
                    binding.edtExpression.setSelection(cursorPosition + btnText.length)
                }
            }
            container.addView(btn)
        }
    }
}
