class SymbolTable:
    def __init__(self, parent=None):
        self.table = {}
        self.parent = parent

    def assign(self, name, value):
        if name in self.table:
            raise NameError(f"'{name}' já foi declarado neste escopo")
        self.table[name] = value

    def set(self, name, value):
        if name in self.table:
            self.table[name] = value
        elif self.parent:
            self.parent.set(name, value)
        else:
            raise NameError(f"'{name}' não foi declarado")

    def get(self, name):
        if name in self.table:
            return self.table[name]
        elif self.parent:
            return self.parent.get(name)
        else:
            raise NameError(f"'{name}' não foi declarado")

    def exists(self, name):
        if name in self.table:
            return True
        elif self.parent:
            return self.parent.exists(name)
        else:
            return False

    def exists_in_current_scope(self, name):
        return name in self.table

    def create_child_scope(self):
        return SymbolTable(parent=self)