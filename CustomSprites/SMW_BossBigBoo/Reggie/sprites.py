class SpriteImage_smwBossBigBoo(SLib.SpriteImage):
    def __init__(self, parent):
        super().__init__(parent, 1.5)
        self.spritebox.shown = False

        self.aux.append(SLib.AuxiliaryImage(parent, 243, 248))
        self.aux[0].image = ImageCache['BigBoo']
        self.aux[0].setPos(-48, -48)
        self.aux[0].hover = False

        self.dimensions = (-38, -80, 98, 102)

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('BigBoo', 'bigboo.png')