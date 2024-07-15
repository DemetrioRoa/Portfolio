import PySimpleGUI as sg
import json
import math

BUTTONS = {                                                  #Solo definimos que hace cada boton
    'log': math.log, 'ln': lambda x: math.log(x, math.e),
    'sin': math.sin, 'cos': math.cos,
    'tan': math.tan, 'cot': lambda x: 1 / math.sin(x),
    'sec': lambda x: 1 / math.cos(x), 'csc': lambda x: 1 / math.sin(x),
    '√': math.sqrt,
    'π': math.pi, 'e': math.e
}
OPERATORS = {                                               #Solo definimos que hace cada operador matematico
    '+': lambda x, y: x + y,
    '-': lambda x, y: x - y,
    '*': lambda x, y: x * y,
    '/': lambda x, y: x / y,
    '^': lambda x, y: x ** y,
}
ORDER_OF_OPERATIONS = [                                    #Aqui solo se pone el orden de operaciones
    (BUTTONS['log'], BUTTONS['ln'], BUTTONS['sin'], BUTTONS['cos'], BUTTONS['tan'], BUTTONS['cot'], BUTTONS['sec'],
     BUTTONS['csc']),
    (OPERATORS['^'], BUTTONS['√']),
    (OPERATORS['*'], OPERATORS['/']),
    (OPERATORS['+'], OPERATORS['-'])
]                                                           #Aqui solo se ponen las operaciones que se realizan al final
INPUTS_AFTER_FUNCTION = (BUTTONS['log'],)
SINGLE_INPUT = (BUTTONS['ln'], BUTTONS['sin'], BUTTONS['cos'], BUTTONS['tan'], BUTTONS['cot'], BUTTONS['sec'],
     BUTTONS['csc'], BUTTONS['√'])
OPENS = '([{'       #Aqui solo ponemos que simbolos abren una operacion
CLOSES = ')]}'      #Aqui solo ponemos que simbolos cierran una operacion


class Formula:
    def __init__(self, name, text, unit): #Creamos objetos para esta clase y por lo tanto para el resto del programa
        self.name = name
        self.text = text
        self.unit = unit
        self.solve_for = None
        self.structure = []
        self.variables = set()
        self.interpret()

    def find(self, context): #Le dice al programa que buscamos
        structure = self.structure
        for i in context:
            structure = structure[i]
        return structure

    def add(self, context, item): #Definimos variables
        if isinstance(item, str):
            self.variables.add(item)
        structure = self.structure
        for i in context:
            structure = structure[i]
        structure.append(item)

    def interpret(self): #Definimos funciones varias de inputs para decirle al programa como interpretar cada cosa que se escribe
        current = ''
        current_type = None
        context = []
        text = self.text.replace(' ', '')
        if '=' in text:
            self.solve_for, text = self.text.split('=')
        for i, c in enumerate(text):
            if c.isalpha():
                if current_type is not str:
                    if current_type is not None:
                        self.add(context, current_type(current))
                        if current_type is float:
                            self.add(context, OPERATORS['*'])
                    current, current_type = c, str
                else:
                    current += c
                if current in BUTTONS: #Aqui por ejemplo se define que de presionar boton definir el inpunt como la funcion del boton
                    self.add(context, BUTTONS[current])
                    current, current_type = '', None
            elif c.isdigit():
                if current_type is not float: #Aqui por otro lado se define que si no es numero, y no es vacio el valor sera incognita y se multiplica
                    if current_type is not None:
                        self.add(context, current_type(current))
                        if current_type is str:
                            self.add(context, OPERATORS['*'])
                    current, current_type = c, float
                else:
                    current += c
            elif c in OPERATORS: #Aqui definimos que cuando se ponga un operador se debe realizar la operacion correspondiente
                if current:
                    self.add(context, current_type(current))
                    current, current_type = '', None
                self.add(context, OPERATORS[c])
            elif c in OPENS: #Aqui definimos la funcion de marcar el inicio de una operacion con parentesis
                if current:
                    if current_type is str and current in BUTTONS:
                        self.add(context, BUTTONS[current])
                    else:
                        self.add(context, current_type(current))
                        if current_type is float:
                            self.add(context, OPERATORS['*'])
                    current, current_type = '', None
                self.add(context, [])
                context.append(len(self.find(context)) - 1)
            elif c in CLOSES: #Aqui efinimos la funcion de marcar el fin de una operacion con parentesis
                if current:
                    self.add(context, current_type(current))
                    current, current_type = '', None
                context.pop(-1)
                if i + 1 < len(text):
                    next_c = text[i + 1]
                    if next_c.isdigit() or next_c.isalpha() or next_c in OPENS:
                        self.add(context, OPERATORS['*'])
            elif c == ',': #Aqui se define el valor de una coma como nada
                if current:
                    self.add(context, current_type(current))
                    current, current_type = '', None
            elif c == '.': #Aqui se define como un punto marca la diferencia de unidades a decimales
                if not current:
                    current += '0'
                    current_type = str
                current += c
            elif c != ' ':
                if not current:
                    current_type = str
                current += c
        if current:
            self.add(context, current_type(current))

    def solve(self, variables, structure=None): #Aqui empezamos a decirle al programa como reaccionar a distintas situaciones
        if any(v not in self.variables for v in variables): #Aqui por ejemplo la primera situacion es si no se dieron todas las variables para resolver la ecuacion
            raise ValueError('Not all required variables given to solve the equation!')
        if structure is None: #Incluso si la estructura es nada se va a regresar a revisar la misma
            structure = self.structure
        for ops in ORDER_OF_OPERATIONS: #Aqui nos dice que las distintas operaciones deben seguir el orden descrito antes
            i = 0
            while i < len(structure):
                item = structure[i]
                if item in ops:
                    if item in SINGLE_INPUT:
                        op = structure[i + 1]
                        if isinstance(op, list):
                            op = self.solve(variables, op)
                            if isinstance(op, str):
                                return op
                        elif isinstance(op, str):
                            op = variables[op]
                        elif not isinstance(op, (float, int)): #Aqui decimos que si no se agregan Valores numericos no es valida la ecuacion
                            return 'Invalid Equation!'
                        result = item(op)
                        structure[i:i + 2] = [result]
                    else:
                        if item in INPUTS_AFTER_FUNCTION: #Si no es una de las funciones basicas se seguira con las operaciones que vienen despues
                            i += 1
                            op_1, op_2 = structure[i:i + 2] = structure[i]
                        else:
                            op_1, _, op_2 = structure[i - 1:i + 2]
                        if isinstance(op_1, list):
                            op_1 = self.solve(variables, op_1)
                            if isinstance(op_1, str):
                                return op_1
                        elif isinstance(op_1, str):
                            op_1 = variables[op_1]
                        elif not isinstance(op_1, (float, int)): #Aqui decimos que si no se agregan Valores numericos no es valida la ecuacion, igual que antes
                            return 'Invalid Equation!'
                        if isinstance(op_2, list):
                            op_2 = self.solve(variables, op_2)
                            if isinstance(op_2, str):
                                return op_2
                        elif isinstance(op_2, str):
                            op_2 = variables[op_2]
                        elif not isinstance(op_2, (float, int)): #Aqui decimos que si no se agregan Valores numericos no es valida la ecuacion
                            return 'Invalid Equation!'
                        result = item(op_1, op_2)
                        structure[i - 1:i + 2] = [result]
                else:
                    i += 1
        if len(structure) != 1:
            return 'Invalid Equation!'
        return structure[0]


class Calculator: #Aqui empezamos a definir un poco lo visual y que mostrar en que situaciones (con esto me refiero a dar click)
    def __init__(self): #Aqui se esta trabajando con la capacidad de almacenar y cargar formulas del archivo de texto
        self.formula = None
        with open('formulas.txt', 'r') as f:
            self.saved_formulas = json.load(f)

        buttons = [ #Aqui claramente se esta creando los botones basicos para crear y calcular formulas, y su formato
            sg.Combo(list(self.saved_formulas.keys()), default_value='Load Formula', key='load', size=20,
                     readonly=True, enable_events=True),
            sg.Button('Create Formula', key='create'),
            sg.Button('Calculate Formula', key='calculate', pad=((5, 112), 3)),
        ]
        for button in BUTTONS:
            buttons.append(sg.Button(button, key=button))

        layout = [ #Aqui se dice que texto aparecera en que parte inicialmente, asi como el formato de este
            [sg.Frame('', [buttons])],
            [sg.Frame('', [[
                sg.Text('Formula:'),
                sg.Text('No loaded formula!', key='formula', size=(102, 1))
            ]])],
            [sg.Frame('', [[
                sg.Text('Output:  '),
                sg.Text('No formula used!', key='output', size=(102, 1))
            ]])],
            [sg.Frame('', [[
                sg.Text('Information Input:', key='ask', size=15),
                sg.Input('No information being asked for right now!', key='input', size=99, disabled=True,
                         pad=((5, 8), 3)),
                sg.Button('Enter', key='enter', bind_return_key=True),
            ]])],
        ]
        self.window = sg.Window("FormDi-DMRP ", layout, background_color='magenta')

    def save(self, file_name='formulas.txt'): #Como se va a llamar el archivo en el que se guardan y abren formulas
        with open(file_name, 'w') as f:
            json.dump(self.saved_formulas, f)

    def create(self):
        # Esta es la parte del crador de formulas y mayor parte del texto general que el usuario vera, y como interactua con la interfaz y respuestas
        self.window['input'].update('', disabled=False)
        self.window['ask'].Update('Enter formula name: ')
        while True:
            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                quit()
            elif event == 'enter' or event == 'input':
                name = values['input']
                break
        self.window['input'].Update('')
        self.window['ask'].Update('Enter formula: ')
        while True:
            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                quit()
            elif event == 'enter' or event == 'input':
                text = values['input']
                break
            elif event in BUTTONS:
                text = self.window['input'].Get()
                self.window['input'].Update(text + event)
        self.window['input'].Update('')
        self.window['ask'].Update('Enter unit: ')
        while True:
            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                quit()
            elif event == 'enter' or event == 'input':
                unit = values['input']
                break
        self.window['input'].update('No information being asked for right now!', disabled=True)
        self.window['ask'].Update('Information Input:')
        self.window['formula'].Update(text)
        self.window['output'].Update('Press the calculate button to see the output!')
        self.formula = Formula(name, text, unit)
        self.saved_formulas[name] = [text, unit]
        self.save()
        self.window['load'].Update('Load Formula', values=list(self.saved_formulas.keys()))

    def calculate(self):
        # Esto es similar que lo anterior solo que para la parte de la calculadora, aqui tambien se define que texto se va a ver, asi como las interacciones con la interfaz y las distintas respuestas del programa
        if self.formula is None:
            self.window['output'].Update('Load or create a formula to be able to calculate it!')
        else:
            self.window['output'].Update('Enter the values of the variables and the result will show!')
            self.window['input'].update('', disabled=False)
            variables = {}
            for variable in sorted(self.formula.variables):
                self.window['ask'].Update(f'Enter value of "{variable}": ')
                while True:
                    event, values = self.window.read()
                    if event == sg.WIN_CLOSED:
                        quit()
                    elif event == 'enter' or event == 'input':
                        self.window['input'].Update('')
                        try:
                            variables[variable] = float(values['input'])
                        except ValueError:
                            self.window['Output'].Update('Input is not numeric, try again!')
                        else:
                            break
            self.window['input'].update('No information being asked for right now!', disabled=True)
            self.window['ask'].Update('Information Input:')
            output = self.formula.solve(variables)
            if not isinstance(output, str):
                if self.formula.solve_for is not None:
                    output = f'{self.formula.solve_for} = {output} {self.formula.unit}'
                else:
                    output = f'{output} {self.formula.unit}'
            self.window['output'].Update(output)

    def load(self, formula):
        # Esto activa la funcion de cargar Formula
        self.formula = Formula(formula, *self.saved_formulas[formula])
        self.window['formula'].Update(self.saved_formulas[formula][0])
        self.window['output'].Update('Press the calculate button to see the output!')

    def run(self):
        # Esto es lo que administra la interfaz y los eventos que va a desencadenar
        while True:
            event, values = self.window.read()
            if event == sg.WIN_CLOSED:
                quit()
            elif event == 'load':
                self.load(values['load'])
            elif event == 'create':
                self.create()
            elif event == 'calculate':
                self.calculate()


calculator = Calculator()
calculator.run()
