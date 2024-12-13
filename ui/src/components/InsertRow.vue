<script setup lang="ts">

import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger
} from '@/components/ui/dialog'
import { Label } from '@/components/ui/label'
import {
    NumberField,
    NumberFieldContent,
    NumberFieldDecrement,
    NumberFieldIncrement,
    NumberFieldInput,
} from '@/components/ui/number-field'
import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import { Plus } from 'lucide-vue-next'

import { cbFetch } from '@/services/api-service';
import { ref } from 'vue';
import type { Cols } from '@/utils/types';

defineProps<{
    tableName: string
    cols: Cols[]
}>()

const error = ref('');

async function insertRow() {

}

function getInputType(type: string): string {
    if (type === 'BLOB') return 'file';
    return 'text'
}

const close = () => { error.value = ""; }
</script>

<template>
    <Dialog>

        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Button variant="outline">
                            <Plus />
                        </Button>
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Insert a row</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>
        <DialogContent>
            <DialogHeader>
                <DialogTitle>Insert a new row into {{ tableName }}</DialogTitle>
                <DialogDescription>

                </DialogDescription>
            </DialogHeader>

            <template v-for="col of cols">
                <NumberField v-if="['INTEGER', 'REAL'].includes(col.type)" :id="col.name"
                    :format-options="col.type === 'REAL' ? {
                        minimumFractionDigits: 2,
                    } : {}">
                    <Label :for="col.name">{{ col.name }}</Label>
                    <NumberFieldContent>
                        <NumberFieldDecrement />
                        <NumberFieldInput />
                        <NumberFieldIncrement />
                    </NumberFieldContent>
                </NumberField>
                <Input v-else :type="getInputType(col.type)" :disabled="!col.type" :placeholder="col.name" />
            </template>

            <pre v-if="error" class="text-destructive">{{ error }}</pre>

            <DialogFooter>
                <DialogClose as-child>
                    <Button @click="close" type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="insertRow">Confirm</Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>