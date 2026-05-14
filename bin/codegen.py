#!/usr/bin/python
# Random Aji's code generator
# Since: 2021/08/09
import random

class Context:
    def __init__(self):
        self.code = ''
        self.alphas = 'abcdefghijklmnopqrstuvwxyz'
        self.max_depth = 4
        self.indent = '  '
        self.idents = {}

    def get(self):
        return self.code

    def add(self, code):
        self.code += code

    def add_indent(self, dep):
        self.code += self.indent * dep

    def gen_random_uniq_iden(self, type, obj):
        n = random.randint(4, 10)
        while True:
            idn = ''
            for _ in range(n):
                idn += random.choice(self.alphas)
            if idn in self.idents.keys():
                continue
            self.idents[idn] = {
                'type': type,
                'obj': obj,
            }
            break
        return idn

    def get_random_struct_obj(self):
        objs = []
        for v in self.idents.values():
            if v['type'] == 'struct':
                objs.append(v['obj'])
        if not len(objs):
            return None
        return random.choice(objs)

class Expr:
    def __init__(self, ctx, gen):
        self.ctx = ctx
        self.gen_ = gen

    def begin(self, dep):
        self.ctx.add_indent(dep)

    def end(self, dep):
        self.ctx.add('\n')

    def gen(self, dep=0):
        n = random.randint(0, 1)
        if n == 0:
            stobj = self.ctx.get_random_struct_obj()
            vaidn = self.ctx.gen_random_uniq_iden(
                type='var', obj=None
            )
            self.ctx.add(f'{vaidn} = {stobj.ident}()')
        else:
            self.ctx.add('1 + 1')

class CodeBlock:
    def __init__(self, ctx, gen):
        self.ctx = ctx
        self.gen_ = gen

    def begin(self, dep):
        self.ctx.add_indent(dep)
        self.ctx.add('{@\n')

    def end(self, dep):
        self.ctx.add_indent(dep)
        self.ctx.add('@}\n')

    def gen(self, lim=4, dep=0):
        doit = random.randint(0, 1)
        if not doit:
            return

        if dep >= self.ctx.max_depth:
            return

        dep = dep + 1
        for _ in range(lim):
            obj = self.gen_.gen(excludes=[CodeBlock])
            obj.begin(dep=dep)
            obj.gen(dep=dep)
            obj.end(dep=dep)

class Struct:
    def __init__(self, ctx, gen):
        self.ctx = ctx
        self.gen_ = gen
        self.ident = self.ctx.gen_random_uniq_iden(
            type='struct', obj=self
        )

    def begin(self, dep):
        self.ctx.add_indent(dep)
        self.ctx.add(f'struct {self.ident}:\n')

    def end(self, dep):
        self.ctx.add_indent(dep)
        self.ctx.add(f'end\n')

    def gen(self, dep, lim=4):
        doit = random.randint(0, 1)
        if not doit:
            return

        if dep >= self.ctx.max_depth:
            return

        dep = dep + 1
        for _ in range(lim):
            obj = self.gen_.gen(excludes=[CodeBlock])
            obj.begin(dep=dep)
            obj.gen(dep=dep)
            obj.end(dep=dep)

class Generator:
    def __init__(self, ctx):
        self.ctx = ctx
        self.classes = [
            Expr,
            CodeBlock,
            Struct,
        ]

    def gen(self, excludes=[]):
        classes = []
        for clas in self.classes:
            if clas not in excludes:
                classes.append(clas)
        clas = random.choice(classes)
        return clas(self.ctx, self)

    def gen_clode_block(self):
        return CodeBlock(self.ctx, self)

def main():
    ctx = Context()
    gen = Generator(ctx)

    obj = gen.gen_clode_block()
    obj.begin(dep=0)
    obj.gen(dep=0)
    obj.end(dep=0)

    print(ctx.get())

if __name__ == '__main__':
    main()
