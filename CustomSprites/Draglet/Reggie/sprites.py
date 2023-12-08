class SpriteImage_Draglet(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)
        self.offset = (-4, -3)

    @staticmethod
    def loadImages():
        if 'Draglet0' in ImageCache: return
        for i in range(8):
            ImageCache[f'Draglet{i}'] = SLib.GetImg(f'draglet{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] & 0xF) % 8

        self.image = ImageCache[f'Draglet{color}']

        super().dataChanged()