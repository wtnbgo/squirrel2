# squirrel wamsoft custom

Author: �n粍�(go@wamsoft.jp)
Date: 2024/3/19

## ���

�g�ݍ��݌��� squirrel (http://squirrel-lang.org/) �̃J�X�^���łł��B
���ۂɃQ�[���G���W���Ȃǂɑg�ݍ��ލۂɒ������������Ȃǂ��܂Ƃ߂�
����܂��B

�ȉ��̃R�[�h������܂�

sqrat  C++�p��squirrel�o�C���h�c�[��
 http://sourceforge.net/projects/scrat/
 ����������Ă��܂�

## MANIFEST

    include/	squirrel�w�b�_�t�@�C��
    squirrel/	squirrel�{�̃\�[�X
    sqstdlib/	squirrel�W�����C�u�����\�[�X
    
    sq/			sq �R�}���h
    sqrat/		sqrat �@�\�ǉ���
    sqobject/	Object/Thread���C�u����
    
    �@�\�ǉ�.txt	squirrel�̎d�l�ύX/�ǉ��@�\�̉��
    �ǂ�ł�.txt	���̃t�@�C��
    
    CMakeLists.txt  �r���h�p

���R���p�C��

cmake �Ńr���h�ł��܂�

    cmake -B build
    cmake --build build

�����C�Z���X

zlib���C�Z���X�� squirrel 2.2.4 ����� fork �ɂȂ�܂�

squirrel/sqrat ���l ��
zlib���C�Z���X�ɏ]���ė��p���Ă��������B

copyright (c)2024 Go Watanabe go@wamsoft.jp
zlib license
