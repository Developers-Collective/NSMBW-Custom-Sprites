class SpriteImage_Grrrol(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(
            parent,
            1.5,
            ImageCache['Grrrol'],
            (0, 0)
        )

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('Grrrol', 'grrrol.png')
        SLib.loadIfNotInImageCache('MegaGrrrol', 'mega_grrrol.png')

    def dataChanged(self):
        isMega = (self.parent.spritedata[2] >> 4) & 0xF

        if isMega == 1:
            self.image = ImageCache['MegaGrrrol']
        else:
            self.image = ImageCache['Grrrol']

        self.offset = (0, 0)

        super().dataChanged()


class SpriteImage_GrrrolSpawner(SLib.SpriteImage_StaticMultiple):  # YYY
    def __init__(self, parent):
        super().__init__(parent, 1.5)

    @staticmethod
    def loadImages():
        names = (
            'down', 'left', 'right',
            'middle_down_0', 'middle_down_1',
            'middle_left_0', 'middle_left_1',
            'middle_right_0', 'middle_right_1',
        )
        for name in names:
            key = f'GrrrolSpawner_{name}'
            SLib.loadIfNotInImageCache(key, f'grrrol_spawner_{name}.png')

    def dataChanged(self):
        data = self.parent.spritedata
        pipe_length = data[2] & 0xF
        direction = (data[3] >> 4) & 0x3

        if direction == 2:
            end = ImageCache['GrrrolSpawner_down']
            middle = (
                ImageCache['GrrrolSpawner_middle_down_0'],
                ImageCache['GrrrolSpawner_middle_down_1'],
            )
            image = QtGui.QPixmap(end.width(), end.height() + pipe_length * middle[0].height())
            image.fill(QtCore.Qt.GlobalColor.transparent)

            painter = QtGui.QPainter(image)
            for i in range(pipe_length):
                y = i * middle[i & 1].height()
                painter.drawPixmap(0, y, middle[i & 1])
            painter.drawPixmap(0, pipe_length * middle[0].height(), end)
            painter.end()
        else:
            if direction == 0:
                end = ImageCache['GrrrolSpawner_left']
                middle = (
                    ImageCache['GrrrolSpawner_middle_left_0'],
                    ImageCache['GrrrolSpawner_middle_left_1'],
                )
                end_on_right = False
            else:
                end = ImageCache['GrrrolSpawner_right']
                middle = (
                    ImageCache['GrrrolSpawner_middle_right_0'],
                    ImageCache['GrrrolSpawner_middle_right_1'],
                )
                end_on_right = True

            image = QtGui.QPixmap(end.width() + pipe_length * middle[0].width(), end.height())
            image.fill(QtCore.Qt.GlobalColor.transparent)

            painter = QtGui.QPainter(image)
            if end_on_right:
                end_x = pipe_length * middle[0].width()
                for i in range(pipe_length):
                    painter.drawPixmap(i * middle[i & 1].width(), 0, middle[i & 1])
            else:
                end_x = 0
                for i in range(pipe_length):
                    x = end.width() + i * middle[i & 1].width()
                    painter.drawPixmap(x, 0, middle[i & 1])
            painter.drawPixmap(end_x, 0, end)
            painter.end()

        self.image = image
        if direction == 2:
            pipe_height = pipe_length * 16.0
            self.offset = (-4, -8 - pipe_height)
        elif direction == 1:
            pipe_width = pipe_length * 16.0
            self.offset = (-pipe_width, -4)
        else:
            self.offset = (0, -4)
            
        super().dataChanged()
